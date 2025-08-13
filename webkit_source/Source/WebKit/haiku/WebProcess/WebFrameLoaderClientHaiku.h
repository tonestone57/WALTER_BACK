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

#include "WebLocalFrameLoaderClient.h"

namespace WebKit {

class WebFrameLoaderClientHaiku final : public WebLocalFrameLoaderClient {
public:
    WebFrameLoaderClientHaiku(WebFrame&);
    virtual ~WebFrameLoaderClientHaiku() = default;

private:
    void startDownload(const WebCore::ResourceRequest&, const String& suggestedName = String(), WebCore::FromDownloadAttribute = WebCore::FromDownloadAttribute::No) final;
    void convertMainResourceLoadToDownload(WebCore::DocumentLoader*, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&) final;

    void dispatchDecidePolicyForNewWindowAction(const WebCore::NavigationAction&, const WebCore::ResourceRequest&, WebCore::FormState*, const WTF::String&, std::optional<WebCore::HitTestResult>&&, WebCore::FramePolicyFunction&&) final;
    void dispatchWillSendRequest(WebCore::DocumentLoader&, unsigned long, WebCore::ResourceRequest&, const WebCore::ResourceResponse&) final;

    void dispatchDidStartProvisionalLoad() final;
    void dispatchDidReceiveTitle(const WebCore::StringWithDirection&) final;
    void dispatchDidFinishLoad() final;
    void dispatchDidFailProvisionalLoad(const WebCore::ResourceError&) final;
    void dispatchDidFailLoad(const WebCore::ResourceError&) final;

    void platformDispatchOnloadEvents() final;
    void platformCreatePlugin(const WebCore::Plugin::Parameters&, CompletionHandler<void(RefPtr<WebCore::Widget>&&)>&&) final;
    bool platformCanHandleRequest(const WebCore::ResourceRequest&) const final;
    WebCore::ResourceError platformBlockedError(const WebCore::ResourceRequest&) const final;
};

} // namespace WebKit
