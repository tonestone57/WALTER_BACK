/*
 * Copyright (C) 2024 Your Name <you@example.com>
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
 * ARISING IN ANY WAY OUT of THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebFrameLoaderClientHaiku.h"

#include <wtf/TZoneMallocInlines.h>

namespace WebKit {

WebFrameLoaderClientHaiku::WebFrameLoaderClientHaiku(Ref<WebFrame>&& frame)
    : WebLocalFrameLoaderClient(WTFMove(frame))
{
}

void WebFrameLoaderClientHaiku::frameLoaderDestroyed()
{
    // FIXME: Implement.
    WebLocalFrameLoaderClient::frameLoaderDestroyed();
}

Ref<WebCore::FrameNetworkingContext> WebFrameLoaderClientHaiku::createNetworkingContext() const
{
    // FIXME: Implement.
    return WebLocalFrameLoaderClient::createNetworkingContext();
}

void WebFrameLoaderClientHaiku::dispatchDecidePolicyForNavigationAction(const WebCore::NavigationAction& navigationAction, const WebCore::ResourceRequest& request, const WebCore::ResourceResponse& redirectResponse, WebCore::FormState* formState, const String& clientRedirectSourceForHistory, std::optional<WebCore::NavigationIdentifier> navigationID, std::optional<WebCore::HitTestResult>&& hitTestResult, bool hasOpener, WebCore::IsPerformingHTTPFallback isPerformingHTTPFallback, WebCore::SandboxFlags sandboxFlags, WebCore::PolicyDecisionMode policyDecisionMode, WebCore::FramePolicyFunction&& function)
{
    // FIXME: Implement. This should send a message to the UIProcess to decide the policy.
    WebLocalFrameLoaderClient::dispatchDecidePolicyForNavigationAction(navigationAction, request, redirectResponse, formState, clientRedirectSourceForHistory, navigationID, WTFMove(hitTestResult), hasOpener, isPerformingHTTPFallback, sandboxFlags, policyDecisionMode, WTFMove(function));
}

void WebFrameLoaderClientHaiku::dispatchWillSendRequest(WebCore::DocumentLoader* documentLoader, WebCore::ResourceLoaderIdentifier identifier, WebCore::ResourceRequest& request, const WebCore::ResourceResponse& redirectResponse)
{
    // FIXME: Implement. This should send a message to the UIProcess.
    WebLocalFrameLoaderClient::dispatchWillSendRequest(documentLoader, identifier, request, redirectResponse);
}

void WebFrameLoaderClientHaiku::dispatchDidReceiveResponse(WebCore::DocumentLoader* documentLoader, WebCore::ResourceLoaderIdentifier identifier, const WebCore::ResourceResponse& response)
{
    // FIXME: Implement. This should send a message to the UIProcess.
    WebLocalFrameLoaderClient::dispatchDidReceiveResponse(documentLoader, identifier, response);
}

void WebFrameLoaderClientHaiku::dispatchDidFinishLoading(WebCore::DocumentLoader* documentLoader, WebCore::IsMainResourceLoad isMainResourceLoad, WebCore::ResourceLoaderIdentifier identifier)
{
    // FIXME: Implement. This should send a message to the UIProcess.
    WebLocalFrameLoaderClient::dispatchDidFinishLoading(documentLoader, isMainResourceLoad, identifier);
}

void WebFrameLoaderClientHaiku::dispatchDidFailLoading(WebCore::DocumentLoader* documentLoader, WebCore::IsMainResourceLoad isMainResourceLoad, WebCore::ResourceLoaderIdentifier identifier, const WebCore::ResourceError& error)
{
    // FIXME: Implement. This should send a message to the UIProcess.
    WebLocalFrameLoaderClient::dispatchDidFailLoading(documentLoader, isMainResourceLoad, identifier, error);
}

void WebFrameLoaderClientHaiku::startDownload(const WebCore::ResourceRequest& request, const String& suggestedName, WebCore::FromDownloadAttribute fromDownloadAttribute)
{
    // FIXME: Implement. This should send a message to the UIProcess to start a download.
    WebLocalFrameLoaderClient::startDownload(request, suggestedName, fromDownloadAttribute);
}

void WebFrameLoaderClientHaiku::applyWebsitePolicies(WebsitePoliciesData&&)
{
    // FIXME: Implement.
}

} // namespace WebKit
