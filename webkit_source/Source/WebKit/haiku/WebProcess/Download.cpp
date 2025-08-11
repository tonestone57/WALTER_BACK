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
#include "Download.h"

#include "DataReference.h"
#include "WebCoreArgumentCoders.h"
#include "WebErrors.h"
#include "WebPage.h"
#include "WebProcess.h"
#include "WebResourceLoaderMessages.h"
#include "WebDownloadProxyMessages.h"

#include <WebCore/ResourceHandle.h>
#include <WebCore/ResourceRequest.h>
#include <WebCore/ResourceResponse.h>
#include <WebCore/SharedBuffer.h>

namespace WebKit {

Download::Download(WebPage& page, DownloadID downloadID, const WebCore::ResourceRequest& request)
    : m_page(page)
    , m_downloadID(downloadID)
{
    m_handle = WebCore::ResourceHandle::create(page.sessionID(), request, this, false, false, page.corePage()->settings().contentSniffingPolicy(), page.corePage()->settings().dataDetectorTypes(), nullptr);
}

Download::~Download()
{
}

void Download::start()
{
    if (m_handle)
        m_handle->start();
}

void Download::cancel()
{
    if (m_handle)
        m_handle->cancel();
}

IPC::Connection* Download::messageSenderConnection() const
{
    return &WebProcess::singleton().ensureConnection();
}

uint64_t Download::messageSenderDestinationID() const
{
    return m_downloadID.downloadID();
}

void Download::didReceiveResponse(WebCore::ResourceHandle* handle, WebCore::ResourceResponse&& response)
{
    send(Messages::WebDownloadProxy::DidReceiveResponse(response));
}

void Download::didReceiveData(WebCore::ResourceHandle* handle, const WebCore::SharedBuffer& buffer)
{
    send(Messages::WebDownloadProxy::DidReceiveData(IPC::DataReference(buffer)));
}

void Download::didFinishLoading(WebCore::ResourceHandle* handle, const WebCore::NetworkLoadMetrics& metrics)
{
    send(Messages::WebDownloadProxy::DidFinish());
    m_page.downloadManager().removeDownload(m_downloadID);
}

void Download::didFail(WebCore::ResourceHandle* handle, const WebCore::ResourceError& error)
{
    IPC::Attachment attachment;
    send(Messages::WebDownloadProxy::DidFail(error, attachment));
    m_page.downloadManager().removeDownload(m_downloadID);
}

} // namespace WebKit
