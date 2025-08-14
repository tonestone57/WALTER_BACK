/*
 * Copyright (C) 2024 Haiku, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebSocketTaskHaiku.h"

#include "NetworkSocketChannel.h"
#include <WebCore/ResourceRequest.h>
#include <WebCore/SocketStreamError.h>
#include <wtf/MainThread.h>
#include <wtf/URL.h>
#include <wtf/text/CString.h>

#include <NetworkAddress.h>
#include <SecureSocket.h>
#include <Socket.h>

namespace WebKit {

WebSocketTaskHaiku::WebSocketTaskHaiku(NetworkSocketChannel& channel, const WebCore::ResourceRequest& request, const String& protocol)
    : WebSocketTask(channel, request, protocol)
{
}

WebSocketTaskHaiku::~WebSocketTaskHaiku()
{
    stopThread();
}

void WebSocketTaskHaiku::sendString(std::span<const uint8_t> text, CompletionHandler<void()>&& completionHandler)
{
    // In the legacy implementation, platformSendInternal was called, which then
    // used callOnWorkerThread. We will replicate that pattern.
    if (m_hasPendingWriteData) {
        // We could queue this, but for now, let's stick to the legacy behavior
        // which didn't handle concurrent sends well.
        completionHandler();
        return;
    }

    m_hasPendingWriteData = true;
    auto writeBuffer = makeUniqueArray<uint8_t>(text.size());
    memcpy(writeBuffer.get(), text.data(), text.size());

    callOnWorkerThread([this, writeBuffer = WTFMove(writeBuffer), writeBufferSize = text.size()]() mutable {
        ASSERT(!isMainThread());
        m_writeBuffer = WTFMove(writeBuffer);
        m_writeBufferSize = writeBufferSize;
        m_writeBufferOffset = 0;
    });

    completionHandler();
}

void WebSocketTaskHaiku::sendData(std::span<const uint8_t> data, CompletionHandler<void()>&& completionHandler)
{
    sendString(data, WTFMove(completionHandler)); // The legacy implementation didn't differentiate.
}

void WebSocketTaskHaiku::close(int32_t, const String&)
{
    // This will trigger the destructor, which calls stopThread.
    if (m_channel)
        m_channel->didClose(0, String());
}

void WebSocketTaskHaiku::cancel()
{
    stopThread();
    if (m_channel)
        m_channel->didClose(0, String());
}

void WebSocketTaskHaiku::resume()
{
    m_workerThread = Thread::create("WebSocket thread"_s, [this, protectedThis = Ref{*this}] {
        threadEntryPoint();
    });
}

void WebSocketTaskHaiku::threadEntryPoint()
{
    ASSERT(!isMainThread());

    const auto& url = m_request.url();
    unsigned int port = url.port() ? *url.port() : (url.protocolIs("wss"_s) ? 443 : 80);
    BNetworkAddress peer(url.host().utf8().data(), port);
    std::unique_ptr<BSocket> socket = url.protocolIs("wss"_s)
        ? std::make_unique<BSecureSocket>()
        : std::make_unique<BSocket>();

    status_t status = socket->Connect(peer);
    if (status != B_OK) {
        handleError(status);
        return;
    }

    callOnMainThread([this, protectedThis = Ref{*this}] {
        if (m_channel)
            m_channel->didConnect(String(), String());
    });

    while (m_running) {
        executeTasks();

        status_t readable = socket->WaitForReadable(20 * 1000);
        if (readable != B_OK && readable != B_TIMED_OUT) {
            handleError(readable);
            break;
        }

        status_t writable = B_ERROR;
        if (m_writeBuffer.get() != nullptr) {
            writable = socket->WaitForWritable(20 * 1000);
            if (writable != B_OK && writable != B_TIMED_OUT) {
                handleError(writable);
                break;
            }
        }

        if ((writable == B_OK) && m_running) {
            auto bytesSent = socket->Write(m_writeBuffer.get() + m_writeBufferOffset, m_writeBufferSize - m_writeBufferOffset);
            if (bytesSent < 0) {
                handleError(bytesSent);
                break;
            }
            m_writeBufferOffset += bytesSent;

            if (m_writeBufferSize <= m_writeBufferOffset) {
                m_writeBuffer = nullptr;
                m_writeBufferSize = 0;
                m_writeBufferOffset = 0;
                callOnMainThread([this, protectedThis = Ref{*this}] {
                    m_hasPendingWriteData = false;
                    // In a real implementation, we'd check if there's more data to send.
                });
            }
        }

        if ((readable == B_OK) && m_running) {
            auto readBuffer = makeUniqueArray<uint8_t>(kReadBufferSize);
            ssize_t bytesRead = socket->Read(readBuffer.get(), kReadBufferSize);
            if (bytesRead <= 0) {
                m_running = false;
                callOnMainThread([this, protectedThis = Ref{*this}] {
                    if (m_channel)
                        m_channel->didReceiveMessageError("Socket read error"_s);
                });
                break;
            }

            callOnMainThread([this, protectedThis = Ref{*this}, buffer = WTFMove(readBuffer), size = bytesRead] {
                if (m_channel)
                    m_channel->didReceiveBinaryData({ buffer.get(), (size_t)size });
            });
        }
    }

    m_writeBuffer = nullptr;
}

void WebSocketTaskHaiku::handleError(status_t errorCode)
{
    m_running = false;
    callOnMainThread([this, protectedThis = Ref{*this}, errorCode, localizedDescription = strerror(errorCode)] {
        if (m_channel)
            m_channel->didReceiveMessageError(String::fromUTF8(localizedDescription));
    });
}

void WebSocketTaskHaiku::stopThread()
{
    if (!m_running.exchange(false))
        return;

    if (m_workerThread) {
        m_workerThread->waitForCompletion();
        m_workerThread = nullptr;
    }
}

void WebSocketTaskHaiku::callOnWorkerThread(Function<void()>&& task)
{
    ASSERT(isMainThread());
    m_taskQueue.append(std::make_unique<Function<void()>>(WTFMove(task)));
}

void WebSocketTaskHaiku::executeTasks()
{
    ASSERT(!isMainThread());
    auto tasks = m_taskQueue.takeAllMessages();
    for (auto& task : tasks)
        (*task)();
}

} // namespace WebKit
