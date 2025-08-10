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

#include "WebPageProxy.h"
#include <memory>

namespace WebKit {

class WebView;

#include "PageConfiguration.h"
#include "IconLoadingClientHaiku.h"

class WebPageProxyHaiku final : public WebPageProxy {
public:
    static Ref<WebPageProxy> create(PageConfiguration&);
    ~WebPageProxyHaiku();

    WebView* view() const;

    void loadURL(const String&);
    void reload();
    void goBack();
    void goForward();
    void stopLoading();

    void setViewNeedsDisplay(const WebCore::Region&);

    const String& mainFrameTitle() const;
    const String& mainFrameURL() const;
    double estimatedProgress() const;
    WebCore::Image* favicon() const;

private:
    void createView();

    // IPC::MessageReceiver
    void didReceiveMessage(IPC::Connection&, IPC::Decoder&) override;

    // WebPageProxy
    void didReceiveTitleForFrame(WebCore::FrameIdentifier, const String&, const UserData&) override;
    void didCommitLoadForFrame(WebCore::FrameIdentifier, FrameInfoData&&, WebCore::ResourceRequest&&, std::optional<WebCore::NavigationIdentifier>, String&&, bool, WebCore::FrameLoadType, const WebCore::CertificateInfo&, bool, String&&, WebCore::ResourceResponseSource, bool, WebCore::HasInsecureContent, WebCore::MouseEventPolicy, const UserData&) override;
    void didStartProgress() override;
    void didChangeProgress(double) override;
    void didFinishProgress() override;

    std::unique_ptr<WebView> m_view;
    String m_mainFrameTitle;
    String m_mainFrameURL;
    double m_estimatedProgress;
    std::unique_ptr<IconLoadingClientHaiku> m_iconLoadingClient;
};

} // namespace WebKit
