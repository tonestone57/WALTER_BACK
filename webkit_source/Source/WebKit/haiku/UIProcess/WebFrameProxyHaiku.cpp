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
#include "WebFrameProxyHaiku.h"

#include "API/FrameInfo.h"
#include "FindOptions.h"
#include "WebPageProxy.h"
#include <WebCore/NotImplemented.h>

namespace WebKit {

Ref<WebFrameProxy> WebFrameProxyHaiku::create(WebPageProxy& page, WebCore::FrameIdentifier frameID)
{
    return adoptRef(*new WebFrameProxyHaiku(page, frameID));
}

WebFrameProxyHaiku::WebFrameProxyHaiku(WebPageProxy& page, WebCore::FrameIdentifier frameID)
    : WebFrameProxy(page, frameID)
{
}

WebFrameProxyHaiku::~WebFrameProxyHaiku()
{
}

void WebFrameProxyHaiku::loadURL(const String& url)
{
    loadRequest(URL(url));
}

void WebFrameProxyHaiku::loadData(const IPC::DataReference& data, const String& mimeType, const String& encoding, const String& baseURL)
{
    if (!m_page)
        return;

    m_page->process().send(Messages::WebPage::LoadData(data, mimeType, encoding, baseURL), m_page->identifier());
}

void WebFrameProxyHaiku::loadAlternateHTML(const IPC::DataReference& html, const String& baseURL, const String& unreachableURL)
{
    if (!m_page)
        return;

    m_page->process().send(Messages::WebPage::LoadAlternateHTML(html, baseURL, unreachableURL), m_page->identifier());
}

void WebFrameProxyHaiku::stopLoading()
{
    WebFrameProxy::stopLoading();
}

void WebFrameProxyHaiku::reload()
{
    WebFrameProxy::reload();
}

void WebFrameProxyHaiku::goBack()
{
    WebFrameProxy::goBack();
}

void WebFrameProxyHaiku::goForward()
{
    WebFrameProxy::goForward();
}

String WebFrameProxyHaiku::requestedURL() const
{
    if (auto provisionalUrl = provisionalURL(); !provisionalUrl.isEmpty())
        return provisionalUrl.string();
    return url();
}

String WebFrameProxyHaiku::url() const
{
    return WebFrameProxy::url().string();
}

String WebFrameProxyHaiku::mimeType() const
{
    return WebFrameProxy::mimeType();
}

bool WebFrameProxyHaiku::canCopy() const
{
    notImplemented();
    return false;
}

bool WebFrameProxyHaiku::canCut() const
{
    notImplemented();
    return false;
}

bool WebFrameProxyHaiku::canPaste() const
{
    notImplemented();
    return false;
}

void WebFrameProxyHaiku::copy()
{
    notImplemented();
}

void WebFrameProxyHaiku::cut()
{
    notImplemented();
}

void WebFrameProxyHaiku::paste()
{
    notImplemented();
}

bool WebFrameProxyHaiku::canUndo() const
{
    notImplemented();
    return false;
}

bool WebFrameProxyHaiku::canRedo() const
{
    notImplemented();
    return false;
}

void WebFrameProxyHaiku::undo()
{
    notImplemented();
}

void WebFrameProxyHaiku::redo()
{
    notImplemented();
}

bool WebFrameProxyHaiku::allowsScrolling() const
{
    notImplemented();
    return false;
}

void WebFrameProxyHaiku::setAllowsScrolling(bool)
{
    notImplemented();
}

BPoint WebFrameProxyHaiku::scrollPosition()
{
    notImplemented();
    return BPoint();
}

String WebFrameProxyHaiku::frameSource() const
{
    notImplemented();
    return String();
}

void WebFrameProxyHaiku::setFrameSource(const String&)
{
    notImplemented();
}

void WebFrameProxyHaiku::setTransparent(bool)
{
    notImplemented();
}

bool WebFrameProxyHaiku::isTransparent() const
{
    notImplemented();
    return false;
}

String WebFrameProxyHaiku::innerText() const
{
    notImplemented();
    return String();
}

String WebFrameProxyHaiku::asMarkup() const
{
    if (!m_page)
        return String();

    String markup;
    if (m_page->process().sendSync(Messages::WebPage::GetFrameMarkup(frameID()), Messages::WebPage::GetFrameMarkup::Reply(markup), m_page->identifier()) != IPC::Error::NoError)
        return String();
    return markup;
}

String WebFrameProxyHaiku::externalRepresentation() const
{
    if (!m_page)
        return String();

    String renderTree;
    if (m_page->process().sendSync(Messages::WebPage::GetFrameRenderTree(frameID()), Messages::WebPage::GetFrameRenderTree::Reply(renderTree), m_page->identifier()) != IPC::Error::NoError)
        return String();
    return renderTree;
}

bool WebFrameProxyHaiku::findString(const String& string, uint32_t findOptions)
{
    if (!m_page)
        return false;

    m_page->findString(string, OptionSet<FindOptions>::fromRaw(findOptions), 1000);
    return true;
}

bool WebFrameProxyHaiku::canIncreaseZoomFactor() const
{
    notImplemented();
    return false;
}

bool WebFrameProxyHaiku::canDecreaseZoomFactor() const
{
    notImplemented();
    return false;
}

void WebFrameProxyHaiku::increaseZoomFactor(bool)
{
    notImplemented();
}

void WebFrameProxyHaiku::decreaseZoomFactor(bool)
{
    notImplemented();
}

void WebFrameProxyHaiku::resetZoomFactor()
{
    notImplemented();
}

void WebFrameProxyHaiku::setEditable(bool)
{
    notImplemented();
}

bool WebFrameProxyHaiku::isEditable() const
{
    notImplemented();
    return false;
}

void WebFrameProxyHaiku::setTitle(const String&)
{
    notImplemented();
}

const String& WebFrameProxyHaiku::title() const
{
    return WebFrameProxy::title();
}

const char* WebFrameProxyHaiku::name() const
{
    return WebFrameProxy::frameName().utf8().data();
}

void WebFrameProxyHaiku::runJavaScript(const String& script)
{
    if (!m_page)
        return;

    m_page->runJavaScriptInFrame(frameID(), script, nullptr);
}

JSGlobalContextRef WebFrameProxyHaiku::globalContext() const
{
    notImplemented();
    return nullptr;
}

} // namespace WebKit
