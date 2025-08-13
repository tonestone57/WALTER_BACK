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
#ifndef _B_WEB_DOWNLOAD_H_
#define _B_WEB_DOWNLOAD_H_

#include <Referenceable.h>
#include <String.h>
#include <Path.h>

class BWebDownloadClient;
class BWebPage;

namespace WebKit {
class WebDownloadProxy;
}

class BWebDownload : public BReferenceable {
public:
    virtual						~BWebDownload();

    void						Cancel();

    void						SetClient(BWebDownloadClient* client);
    void                        SetDestination(const BPath& path);

    const BString&				URL() const;
    const BPath&				Path() const;
    const BString&				Filename() const;

    off_t						CurrentSize() const;
    off_t						ExpectedSize() const;

private:
    friend class BWebPage;
    friend class WebKit::WebDownloadProxy;

                                BWebDownload(WebKit::WebDownloadProxy& proxy);

private:
    WebKit::WebDownloadProxy&   fProxy;
    BWebDownloadClient*         fClient;
    BString                     fUrl;
};

class BWebDownloadClient {
public:
    virtual void				DownloadStarted(BWebDownload* download) = 0;
    virtual void				DownloadProgress(BWebDownload* download, off_t bytesReceived, off_t totalBytes) = 0;
    virtual void				DownloadFinished(BWebDownload* download) = 0;
    virtual void				DownloadFailed(BWebDownload* download) = 0;
    virtual void				DownloadCancelled(BWebDownload* download) = 0;
};

#endif // _B_WEB_DOWNLOAD_H_
