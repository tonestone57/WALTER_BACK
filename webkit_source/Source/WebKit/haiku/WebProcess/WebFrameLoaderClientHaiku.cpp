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
#include "WebFrameLoaderClientHaiku.h"

#include "DownloadManager.h"
#include "WebFrame.h"
#include "WebPage.h"
#include "WebPageHaiku.h"
#include "WebPageProxyMessages.h"
#include <WebCore/DocumentLoader.h>
#include <WebCore/Frame.h>
#include <WebCore/FrameLoader.h>
#include <WebCore/FrameLoaderClient.h>
#include <WebCore/MouseEvent.h>
#include <WebCore/NavigationAction.h>
#include <WebCore/NotImplemented.h>
#include <WebCore/ResourceError.h>
#include <WebCore/PluginData.h>
#include <WebCore/UserAgent.h>

namespace WebKit {

using namespace WebCore;

WebFrameLoaderClientHaiku::WebFrameLoaderClientHaiku(WebFrame& frame)
    : WebLocalFrameLoaderClient(frame)
{
}

void WebFrameLoaderClientHaiku::startDownload(const ResourceRequest& request, const String&, FromDownloadAttribute)
{
    if (!m_frame)
        return;

    if (auto* page = m_frame->page())
        static_cast<WebPageHaiku*>(page)->downloadManager().startDownload(request);
}

void WebFrameLoaderClientHaiku::convertMainResourceLoadToDownload(DocumentLoader* documentLoader, const ResourceRequest& request, const ResourceResponse& response)
{
    if (!m_frame)
        return;

    if (auto* page = m_frame->page()) {
        static_cast<WebPageHaiku*>(page)->downloadManager().startDownload(request);
        documentLoader->cancelMainResourceLoad(pluginWillHandleLoadError(response));
    }
}

void WebFrameLoaderClientHaiku::dispatchDecidePolicyForNavigationAction(
    const NavigationAction& action, const ResourceRequest& request,
    FramePolicyFunction&& policyFunction)
{
    if (!request.url().protocolIsInHTTPFamily()) {
        if (auto* page = m_frame->page())
            page->send(Messages::WebPageProxy::LaunchURL(request.url()));
        policyFunction(PolicyAction::Ignore, nullptr);
        return;
    }

    WebLocalFrameLoaderClient::dispatchDecidePolicyForNavigationAction(action, request, WTFMove(policyFunction));
}

void WebFrameLoaderClientHaiku::dispatchDecidePolicyForNewWindowAction(
    const NavigationAction& action, const ResourceRequest& request, FormState*,
    const String& frameName, std::optional<HitTestResult>&&, FramePolicyFunction&& policyFunction)
{
    if (action.mouseEventData() && action.mouseEventData()->button == MouseButton::Middle) {
        if (auto* page = m_frame->page()) {
            page->process().send(Messages::WebPageProxy::CreateNewPage({ }, request), page->identifier());
        }
        policyFunction(PolicyAction::Ignore, nullptr);
        return;
    }

    WebLocalFrameLoaderClient::dispatchDecidePolicyForNewWindowAction(
        action, request, nullptr, frameName, std::nullopt, WTFMove(policyFunction));
}

void WebFrameLoaderClientHaiku::dispatchWillSendRequest(WebCore::DocumentLoader& loader, unsigned long identifier, WebCore::ResourceRequest& request, const WebCore::ResourceResponse& response)
{
    WebLocalFrameLoaderClient::dispatchWillSendRequest(loader, identifier, request, response);
}

void WebFrameLoaderClientHaiku::platformDispatchOnloadEvents()
{
    if (m_frame)
        if (auto* page = m_frame->page())
            page->send(Messages::WebPageProxy::DispatchOnloadEvents());
}

void WebFrameLoaderClientHaiku::platformCreatePlugin(const Plugin::Parameters&, CompletionHandler<void(RefPtr<Widget>&&)>&& completionHandler)
{
    // TODO: Implement
    completionHandler(nullptr);
}

bool WebFrameLoaderClientHaiku::platformCanHandleRequest(const ResourceRequest&) const
{
    // TODO: Implement
    return true;
}

ResourceError WebFrameLoaderClientHaiku::platformBlockedError(const ResourceRequest&) const
{
    // TODO: Implement
    return ResourceError();
}

void WebFrameLoaderClientHaiku::detachedFromParent()
{
}

void WebFrameLoaderClientHaiku::dispatchDidReceiveAuthenticationChallenge(WebCore::DocumentLoader&, uint64_t, const WebCore::AuthenticationChallenge&)
{
    notImplemented();
}

void WebFrameLoaderClientHaiku::dispatchDidReceiveTitle(const WebCore::StringWithDirection& title)
{
    if (auto* page = m_frame->page())
        page->send(Messages::WebPageProxy::DidReceiveTitle(title.string));
}

void WebFrameLoaderClientHaiku::dispatchDidCommitLoad(std::optional<WebCore::HasInsecureContent>, std::optional<WebCore::UsedLegacyTLS>, std::optional<WasPrivateRelayed>)
{
    if (auto* page = m_frame->page())
        page->send(Messages::WebPageProxy::DidCommitLoad());
}

void WebFrameLoaderClientHaiku::dispatchDidFinishLoad()
{
    if (auto* page = m_frame->page())
        page->send(Messages::WebPageProxy::DidFinishLoad());
}

void WebFrameLoaderClientHaiku::dispatchDidFailLoad(const WebCore::ResourceError& error)
{
    if (auto* page = m_frame->page()) {
        // FIXME: This message does not exist.
        // page->send(Messages::WebPageProxy::DidFailLoad(error));
    }
}

void WebFrameLoaderClientHaiku::dispatchDidReceiveIcon()
{
    if (auto* page = m_frame->page()) {
        // In WebKitLegacy, this sent a message to the UI process.
        // We need a corresponding IPC message here. Let's assume one exists.
        // page->send(Messages::WebPageProxy::DidReceiveIcon());
    }
}

void WebFrameLoaderClientHaiku::dispatchDidFinishDocumentLoad()
{
    if (auto* page = m_frame->page())
        page->send(Messages::WebPageProxy::DidFinishDocumentLoad());
}

RefPtr<WebCore::LocalFrame> WebFrameLoaderClientHaiku::createFrame(const WTF::AtomString& name, WebCore::HTMLFrameOwnerElement& ownerElement)
{
    notImplemented();
    return nullptr;
}

String WebFrameLoaderClientHaiku::userAgent(const WTF::URL& url) const
{
    return WebCore::standardUserAgent("WebPositive"_s, "2.0"_s);
}

} // namespace WebKit
