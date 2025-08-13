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
#include "WebDownloadProxy.h"

#include "APIUIClient.h"
#include "WebProcessProxy.h"
#include "WebPageProxy.h"
#include "WebProcessMessages.h"
#include "WebDownloadProxyMessages.h"
#include <WebCore/ResourceRequest.h>
#include <WebCore/ResourceResponse.h>
#include <WebCore/ResourceError.h>
#include <wtf/URL.h>

#include <stdio.h>
#include <Directory.h>

namespace WebKit {

Ref<WebDownloadProxy> WebDownloadProxy::create(BWebDownload& bdownload, WebPageProxy& page, DownloadID downloadID, const WebCore::ResourceRequest& request, const String& suggestedFilename)
{
    return adoptRef(*new WebDownloadProxy(bdownload, page, downloadID, request, suggestedFilename));
}

WebDownloadProxy::WebDownloadProxy(BWebDownload& bdownload, WebPageProxy& page, DownloadID downloadID, const WebCore::ResourceRequest& request, const String& suggestedFilename)
    : m_downloadID(downloadID)
    , m_page(page)
    , m_bdownload(&bdownload)
    , m_request(request)
    , m_suggestedFilename(suggestedFilename)
{
    process().addMessageReceiver(Messages::WebDownloadProxy::messageReceiverName(), m_downloadID.downloadID(), *this);
}

WebDownloadProxy::~WebDownloadProxy()
{
    process().removeMessageReceiver(Messages::WebDownloadProxy::messageReceiverName(), m_downloadID.downloadID());
}

WebProcessProxy& WebDownloadProxy::process()
{
    return m_page->process();
}

void WebDownloadProxy::cancel()
{
    process().send(Messages::WebProcess::CancelDownload(m_downloadID), 0);
    platformCancel();
}

void WebDownloadProxy::resume()
{
    process().send(Messages::WebProcess::ResumeDownload(m_downloadID, m_path.Path()), 0);
}

void WebDownloadProxy::setDestination(const String& destination)
{
    m_path.SetTo(destination.utf8().data());
}

void WebDownloadProxy::didStart(const WebCore::ResourceRequest&)
{
    if (m_bdownload && m_bdownload->client())
        m_bdownload->client()->DownloadStarted(m_bdownload);
}

void WebDownloadProxy::didReceiveResponse(const WebCore::ResourceResponse& response)
{
    m_response = response;
    if (m_suggestedFilename.IsEmpty())
        m_suggestedFilename = m_response.suggestedFilename();
    if (m_suggestedFilename.IsEmpty()) {
        WTF::URL url(response.url());
        url.setQuery(String());
        url.removeFragmentIdentifier();
        m_suggestedFilename = PAL::decodeURLEscapeSequences(url.lastPathComponent()).utf8().data();
    }

    if (m_path.InitCheck() != B_OK) {
        // No path provided, use the default downloads folder.
        m_path.SetTo(m_page->preferences().downloadPath());
    }

    BPath filePath = m_path;
    filePath.Append(m_suggestedFilename);

    BEntry entry(filePath.Path());
    for (int32 i = 1; entry.Exists(); i++) {
        BString baseName = m_suggestedFilename;
        int32 extensionStart = baseName.FindLast('.');
        BString extension;
        if (extensionStart > 0)
            baseName.MoveInto(extension, extensionStart, baseName.CountChars() - extensionStart);
        char num[12];
        snprintf(num, sizeof(num), "-%" B_PRId32, i);
        baseName.Append(num).Append(extension);
        filePath.SetTo(m_path.Path());
        filePath.Append(baseName);
        entry.SetTo(filePath.Path());
    }
    m_path = filePath;

    if (m_file.SetTo(m_path.Path(), B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY) != B_OK) {
        didFail(WebCore::ResourceError(String(), 0, URL(), "Failed to create download file"), { });
        return;
    }
}

void WebDownloadProxy::didReceiveData(const IPC::DataReference& data, uint64_t)
{
    m_file.Write(data.data(), data.size());
    m_bytesReceived += data.size();

    if (m_bdownload->client())
        m_bdownload->client()->DownloadProgress(m_bdownload, m_bytesReceived, m_response.expectedContentLength());
}

void WebDownloadProxy::didFinish()
{
    platformDidFinish();
    process().send(Messages::WebProcess::ClearDownloadResumeData(m_downloadID), 0);
    if (m_bdownload->client())
        m_bdownload->client()->DownloadFinished(m_bdownload);
}

void WebDownloadProxy::didFail(const WebCore::ResourceError& error, const IPC::DataReference&)
{
    platformDidFinish();
    process().send(Messages::WebProcess::ClearDownloadResumeData(m_downloadID), 0);
    if (m_bdownload->client())
        m_bdownload->client()->DownloadFailed(m_bdownload);
}

void WebDownloadProxy::didCancel(const IPC::DataReference&)
{
    platformCancel();
    if (m_bdownload->client())
        m_bdownload->client()->DownloadCancelled(m_bdownload);
}

void WebDownloadProxy::platformCancel()
{
    m_file.Unset();
    BEntry(m_path.Path()).Remove();
}

void WebDownloadProxy::platformDidFinish()
{
    m_file.Unset();
}

} // namespace WebKit
