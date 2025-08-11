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

#pragma once

#include "APIObject.h"
#include "DownloadID.h"
#include "WebPageProxy.h"
#include <wtf/Forward.h>

#include <private/netservices/UrlContext.h>
#include <File.h>
#include <Path.h>

namespace API {
class Data;
}

namespace WebCore {
class ResourceError;
class ResourceRequest;
class ResourceResponse;
}

namespace WebKit {

class WebProcessProxy;
class WebPageProxy;

class WebDownloadProxy : public API::ObjectImpl<API::Object::Type::Download> {
public:
    static Ref<WebDownloadProxy> create(WebPageProxy&, DownloadID, const WebCore::ResourceRequest&, const String& suggestedFilename = { });
    virtual ~WebDownloadProxy();

    DownloadID downloadID() const { return m_downloadID; }
    WebPageProxy* page() const { return m_page; }

    void cancel();
    void resume();
    void setDestination(const String&);

    void didReceiveResponse(const WebCore::ResourceResponse&);
    void didReceiveData(const IPC::DataReference&, uint64_t);
    void didFinish();
    void didFail(const WebCore::ResourceError&, const IPC::DataReference&);
    void didCancel(const IPC::DataReference&);

private:
    WebDownloadProxy(WebPageProxy&, DownloadID, const WebCore::ResourceRequest&, const String& suggestedFilename);

    WebProcessProxy& process();
    void platformCancel();
    void platformDidFinish();

    DownloadID m_downloadID;
    RefPtr<WebPageProxy> m_page;
    WebCore::ResourceRequest m_request;
    WebCore::ResourceResponse m_response;
    BString m_suggestedFilename;
    BPath m_path;
    BFile m_file;
    uint64_t m_bytesReceived { 0 };
};

} // namespace WebKit
