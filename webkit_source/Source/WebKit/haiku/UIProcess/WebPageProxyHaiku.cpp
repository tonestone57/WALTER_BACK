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
#include "WebPageProxyHaiku.h"

#include "WebView.h"

namespace WebKit {

Ref<WebPageProxy> WebPageProxyHaiku::create(PageConfiguration& configuration)
{
    return adoptRef(*new WebPageProxyHaiku(configuration));
}

WebPageProxyHaiku::WebPageProxyHaiku(PageConfiguration& configuration)
    : WebPageProxy(configuration)
    , m_iconLoadingClient(std::make_unique<IconLoadingClientHaiku>())
    , m_findClient(std::make_unique<FindClientHaiku>())
{
    createView();
    setIconLoadingClient(m_iconLoadingClient.get());
    setFindClient(m_findClient.get());
}

WebPageProxyHaiku::~WebPageProxyHaiku()
{
}

WebView* WebPageProxyHaiku::view() const
{
    return m_view.get();
}

#include <WebCore/Region.h>

void WebPageProxyHaiku::createView()
{
    m_view = std::make_unique<WebView>(*this);
}

#include <WebCore/ResourceRequest.h>

void WebPageProxyHaiku::setViewNeedsDisplay(const WebCore::Region& region)
{
    if (m_view)
        m_view->Invalidate(region.bounds());
}

const String& WebPageProxyHaiku::mainFrameTitle() const
{
    return m_mainFrameTitle;
}

void WebPageProxyHaiku::didReceiveMessage(IPC::Connection& connection, IPC::Decoder& decoder)
{
    WebPageProxy::didReceiveMessage(connection, decoder);
}

#include "FrameInfoData.h"

void WebPageProxyHaiku::didReceiveTitleForFrame(WebCore::FrameIdentifier frameID, const String& title, const UserData&)
{
    if (mainFrame() && mainFrame()->frameID() == frameID)
        m_mainFrameTitle = title;
}

const String& WebPageProxyHaiku::mainFrameURL() const
{
    return m_mainFrameURL;
}

void WebPageProxyHaiku::didCommitLoadForFrame(WebCore::FrameIdentifier frameID, FrameInfoData&& frameInfo, WebCore::ResourceRequest&& request, std::optional<WebCore::NavigationIdentifier> navigationID, String&& mimeType, bool frameHasCustomContentProvider, WebCore::FrameLoadType frameLoadType, const WebCore::CertificateInfo& certificateInfo, bool usedLegacyTLS, String&& proxyName, WebCore::ResourceResponseSource source, bool containsPluginDocument, WebCore::HasInsecureContent hasInsecureContent, WebCore::MouseEventPolicy mouseEventPolicy, const UserData& userData)
{
    WebPageProxy::didCommitLoadForFrame(frameID, WTFMove(frameInfo), WTFMove(request), navigationID, WTFMove(mimeType), frameHasCustomContentProvider, frameLoadType, certificateInfo, usedLegacyTLS, WTFMove(proxyName), source, containsPluginDocument, hasInsecureContent, mouseEventPolicy, userData);

    if (mainFrame() && mainFrame()->frameID() == frameID)
        m_mainFrameURL = frameInfo.url;
}

double WebPageProxyHaiku::estimatedProgress() const
{
    return m_estimatedProgress;
}

void WebPageProxyHaiku::didStartProgress()
{
    m_estimatedProgress = 0.0;
}

void WebPageProxyHaiku::didChangeProgress(double value)
{
    m_estimatedProgress = value;
}

void WebPageProxyHaiku::didFinishProgress()
{
    m_estimatedProgress = 1.0;
}

void WebPageProxyHaiku::loadURL(const String& url)
{
    loadRequest(WebCore::ResourceRequest(URL(url)));
}

void WebPageProxyHaiku::reload()
{
    WebPageProxy::reload(false);
}

void WebPageProxyHaiku::goBack()
{
    WebPageProxy::goBack();
}

void WebPageProxyHaiku::goForward()
{
    WebPageProxy::goForward();
}

void WebPageProxyHaiku::stopLoading()
{
    WebPageProxy::stopLoading();
}

WebCore::Image* WebPageProxyHaiku::favicon() const
{
    return m_iconLoadingClient->favicon();
}

void WebPageProxyHaiku::increaseZoomFactor(bool textOnly)
{
    if (textOnly)
        setTextZoomFactor(textZoomFactor() * 1.2);
    else
        setPageZoomFactor(pageZoomFactor() * 1.2);
}

void WebPageProxyHaiku::decreaseZoomFactor(bool textOnly)
{
    if (textOnly)
        setTextZoomFactor(textZoomFactor() / 1.2);
    else
        setPageZoomFactor(pageZoomFactor() / 1.2);
}

void WebPageProxyHaiku::resetZoomFactor()
{
    setTextZoomFactor(1.0);
    setPageZoomFactor(1.0);
}

void WebPageProxyHaiku::findString(const String& string, OptionSet<FindOptions> options, unsigned maxMatchCount)
{
    WebPageProxy::findString(string, options, maxMatchCount);
}

} // namespace WebKit
