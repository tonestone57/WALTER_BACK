/*
 * Copyright (C) 2024 Your Name <your@email.com>
 *
 * All rights reserved.
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
#include "DownloadProxy.h"

#include "API/Client/Haiku/BWebDownload.h"
#include "API/Client/Haiku/WebDownloadPrivate.h"
#include <Messenger.h>
#include <wtf/HashMap.h>

namespace WebKit {

static HashMap<DownloadID, BWebDownload*> s_downloadMap;

void DownloadProxy::platformDidStart()
{
    BWebDownload* download = new BWebDownload(new BPrivate::WebDownloadPrivate(*this));
    s_downloadMap.set(downloadID(), download);
}

void DownloadProxy::platformDidReceiveData(uint64_t bytesWritten, uint64_t totalBytesWritten, uint64_t totalBytesExpectedToWrite)
{
    if (BWebDownload* download = s_downloadMap.get(downloadID())) {
        if (download->ProgressListener().IsValid()) {
            BMessage progressMessage(B_DOWNLOAD_PROGRESS);
            progressMessage.AddInt64("bytes_received", totalBytesWritten);
            progressMessage.AddInt64("bytes_total", totalBytesExpectedToWrite);
            download->ProgressListener().SendMessage(&progressMessage);
        }
    }
}

void DownloadProxy::platformDidFinish()
{
    if (BWebDownload* download = s_downloadMap.take(downloadID()))
        delete download;
}

void DownloadProxy::platformDidFail(const WebCore::ResourceError&)
{
    if (BWebDownload* download = s_downloadMap.take(downloadID()))
        delete download;
}

void DownloadProxy::platformDidFinish()
{
    if (BWebDownload* download = s_downloadMap.take(downloadID()))
        delete download;
}

void DownloadProxy::platformDidFail(const WebCore::ResourceError&)
{
    if (BWebDownload* download = s_downloadMap.take(downloadID()))
        delete download;
}

void DownloadProxy::platformCancel(std::span<const uint8_t>)
{
    if (BWebDownload* download = s_downloadMap.get(downloadID()))
        download->Cancel();
}

} // namespace WebKit
