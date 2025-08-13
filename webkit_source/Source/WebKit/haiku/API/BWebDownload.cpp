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

#include "BWebDownload.h"

#include "WebDownloadProxy.h"
#include <WebCore/ResourceRequest.h>

BWebDownload::BWebDownload(WebKit::WebDownloadProxy& proxy)
    : fProxy(proxy)
    , fClient(nullptr)
    , fUrl(fProxy.request().url().string().utf8().data())
{
}

BWebDownload::~BWebDownload()
{
}

void BWebDownload::Cancel()
{
    fProxy.cancel();
}

void BWebDownload::SetClient(BWebDownloadClient* client)
{
    fClient = client;
}

void BWebDownload::SetDestination(const BPath& path)
{
    fProxy.setDestination(path.Path());
}

const BString& BWebDownload::URL() const
{
    return fUrl;
}

const BPath& BWebDownload::Path() const
{
    return fProxy.path();
}

const BString& BWebDownload::Filename() const
{
    return fProxy.suggestedFilename();
}

off_t BWebDownload::CurrentSize() const
{
    return fProxy.bytesReceived();
}

off_t BWebDownload::ExpectedSize() const
{
    return fProxy.expectedBytes();
}
