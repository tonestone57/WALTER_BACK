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

#include "config.hh"
#include "WebChromeClientHaiku.h"

#include "WebPage.h"
#include "WebProcess.h"
#include "WebPageProxyMessages.h"
#include <WebCore/ContextMenu.h>
#include <WebCore/FrameLoader.h>
#include <WebCore/WindowFeatures.h>
#include <WebCore/NavigationAction.h>

#include "WebContextMenuItemData.h"
#include "ContextMenuContextData.h"

namespace WebKit {

using namespace WebCore;

WebChromeClientHaiku::WebChromeClientHaiku(WebPage& page)
    : WebChromeClient(page)
{
}

void WebChromeClientHaiku::setWindowRect(const FloatRect& rect)
{
    m_page.process().send(Messages::WebPageProxy::SetWindowRect(rect), m_page.identifier());
}

FloatRect WebChromeClientHaiku::windowRect() const
{
    FloatRect rect;
    m_page.process().sendSync(Messages::WebPageProxy::GetWindowRect(), Messages::WebPageProxy::GetWindowRect::Reply(rect), m_page.identifier());
    return rect;
}

void WebChromeClientHaiku::focus()
{
    m_page.process().send(Messages::WebPageProxy::SetFocus(true), m_page.identifier());
}

void WebChromeClientHaiku::unfocus()
{
    m_page.process().send(Messages::WebPageProxy::SetFocus(false), m_page.identifier());
}

void WebChromeClientHaiku::runJavaScriptAlert(LocalFrame&, const String&)
{
    // TODO: Implement
}

bool WebChromeClientHaiku::runJavaScriptConfirm(LocalFrame&, const String&)
{
    // TODO: Implement
    return false;
}

bool WebChromeClientHaiku::runJavaScriptPrompt(LocalFrame&, const String&, const String&, String&)
{
    // TODO: Implement
    return false;
}

void WebChromeClientHaiku::setStatusbarVisible(bool visible)
{
    m_page.process().send(Messages::WebPageProxy::SetStatusbarVisible(visible), m_page.identifier());
}

bool WebChromeClientHaiku::statusbarVisible() const
{
    bool isVisible = true;
    m_page.process().sendSync(Messages::WebPageProxy::StatusbarIsVisible(), Messages::WebPageProxy::StatusbarIsVisible::Reply(isVisible), m_page.identifier());
    return isVisible;
}

void WebChromeClientHaiku::setToolbarsVisible(bool visible)
{
    m_page.process().send(Messages::WebPageProxy::SetToolbarsVisible(visible), m_page.identifier());
}

bool WebChromeClientHaiku::toolbarsVisible() const
{
    bool isVisible = true;
    m_page.process().sendSync(Messages::WebPageProxy::ToolbarsAreVisible(), Messages::WebPageProxy::ToolbarsAreVisible::Reply(isVisible), m_page.identifier());
    return isVisible;
}

void WebChromeClientHaiku::setMenubarVisible(bool visible)
{
    m_page.process().send(Messages::WebPageProxy::SetMenubarVisible(visible), m_page.identifier());
}

bool WebChromeClientHaiku::menubarVisible() const
{
    bool isVisible = true;
    m_page.process().sendSync(Messages::WebPageProxy::MenubarIsVisible(), Messages::WebPageProxy::MenubarIsVisible::Reply(isVisible), m_page.identifier());
    return isVisible;
}

void WebChromeClientHaiku::setResizable(bool resizable)
{
    m_page.process().send(Messages::WebPageProxy::SetResizable(resizable), m_page.identifier());
}

void WebChromeClientHaiku::closeWindow()
{
    m_page.process().send(Messages::WebPageProxy::ClosePage(), m_page.identifier());
}

void WebChromeClientHaiku::addMessageToConsole(MessageSource source, MessageLevel level, const String& message, unsigned lineNumber, unsigned columnNumber, const String& sourceID)
{
    // TODO: Implement
}

void WebChromeClientHaiku::mouseDidMoveOverElement(const HitTestResult&, OptionSet<PlatformEventModifier>, const String&, TextDirection)
{
    // TODO: Implement
}

void WebChromeClientHaiku::print(LocalFrame& frame)
{
    // TODO: Implement
}

void WebChromeClientHaiku::runOpenPanel(LocalFrame&, FileChooser& chooser)
{
    // TODO: Implement
}

void WebChromeClientHaiku::exceededDatabaseQuota(LocalFrame&, const String&, DatabaseDetails)
{
    // TODO: Implement
}

void WebChromeClientHaiku::didCompletePageLoadForMainFrame()
{
    // TODO: Implement
}

void WebChromeClientHaiku::didFinishLoading()
{
    // TODO: Implement
}

void WebChromeClientHaiku::isPlayingAudioDidChange(bool)
{
    // TODO: Implement
}

RefPtr<Page> WebChromeClientHaiku::createWindow(LocalFrame&, const String&, const WindowFeatures& windowFeatures, const NavigationAction& navigationAction)
{
    auto& page = m_page;
    auto& process = page.process();

    auto newPageProxyIdentifier = process.sendSync(
        Messages::WebPageProxy::CreateNewPage(windowFeatures, navigationAction.resourceRequest()),
        page.identifier())->newWebPageProxyIdentifier();

    if (!newPageProxyIdentifier)
        return nullptr;

    // FIXME: This is not the right way to get the WebPage.
    // It should be created by the UI process and we should get a message back.
    // For now, we assume it exists.
    if (auto* webPage = WebProcess::singleton().webPage(*newPageProxyIdentifier))
        return webPage->corePage();

    return nullptr;
}

void WebChromeClientHaiku::showContextMenu(const ContextMenu* menu) const
{
    Vector<WebContextMenuItemData> menuItems;
    for (const auto& item : menu->items())
        menuItems.append(WebContextMenuItemData(item));

    m_page.process().send(Messages::WebPageProxy::ShowContextMenu(
            m_page.contextMenuContextData(), menuItems), m_page.identifier());
}

} // namespace WebKit
