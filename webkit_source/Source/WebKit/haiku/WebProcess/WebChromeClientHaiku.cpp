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
#include "WebChromeClientHaiku.h"

#include "ContextMenuContextData.h"
#include "HangDetectionDisabler.h"
#include "WebContextMenuItemData.h"
#include "WebOpenPanelResultListener.h"
#include "WebPage.h"
#include "WebPageProxyMessages.h"
#include "WebProcess.h"
#include <WebCore/ContextMenu.h>
#include <WebCore/FileChooser.h>
#include <WebCore/Frame.h>
#include <WebCore/FrameLoader.h>
#include <WebCore/NavigationAction.h>
#include <WebCore/WindowFeatures.h>

namespace WebKit {

using namespace WebCore;

WebChromeClientHaiku::WebChromeClientHaiku(WebPage& page)
    : WebChromeClient(page)
{
}

void WebChromeClientHaiku::setWindowRect(const FloatRect& rect)
{
    m_page.send(Messages::WebPageProxy::SetWindowRect(rect));
}

FloatRect WebChromeClientHaiku::windowRect() const
{
    auto sendResult = m_page.sendSync(Messages::WebPageProxy::GetWindowRect());
    auto [rect] = sendResult.takeReplyOr(FloatRect { });
    return rect;
}

void WebChromeClientHaiku::focus()
{
    m_page.send(Messages::WebPageProxy::SetFocus(true));
}

void WebChromeClientHaiku::unfocus()
{
    m_page.send(Messages::WebPageProxy::SetFocus(false));
}

void WebChromeClientHaiku::runJavaScriptAlert(LocalFrame& frame, const String& message)
{
    HangDetectionDisabler hangDetectionDisabler;
    m_page.prepareToRunModalJavaScriptDialog();
    m_page.sendSync(Messages::WebPageProxy::RunJavaScriptAlert(frame.frameID(), frame.info(), message), Messages::WebPageProxy::RunJavaScriptAlert::Reply());
}

bool WebChromeClientHaiku::runJavaScriptConfirm(LocalFrame& frame, const String& message)
{
    HangDetectionDisabler hangDetectionDisabler;
    m_page.prepareToRunModalJavaScriptDialog();
    auto sendResult = m_page.sendSync(Messages::WebPageProxy::RunJavaScriptConfirm(frame.frameID(), frame.info(), message));
    auto [result] = sendResult.takeReplyOr(false);
    return result;
}

bool WebChromeClientHaiku::runJavaScriptPrompt(LocalFrame& frame, const String& message, const String& defaultValue, String& result)
{
    HangDetectionDisabler hangDetectionDisabler;
    m_page.prepareToRunModalJavaScriptDialog();
    auto sendResult = m_page.sendSync(Messages::WebPageProxy::RunJavaScriptPrompt(frame.frameID(), frame.info(), message, defaultValue));
    auto [ok, promptResult] = sendResult.takeReplyOr(false, String { });
    result = promptResult;
    return ok;
}

void WebChromeClientHaiku::setStatusbarVisible(bool visible)
{
    m_page.send(Messages::WebPageProxy::SetStatusbarVisible(visible));
}

bool WebChromeClientHaiku::statusbarVisible() const
{
    auto sendResult = m_page.sendSync(Messages::WebPageProxy::StatusbarIsVisible());
    auto [isVisible] = sendResult.takeReplyOr(true);
    return isVisible;
}

void WebChromeClientHaiku::setToolbarsVisible(bool visible)
{
    m_page.send(Messages::WebPageProxy::SetToolbarsVisible(visible));
}

bool WebChromeClientHaiku::toolbarsVisible() const
{
    auto sendResult = m_page.sendSync(Messages::WebPageProxy::ToolbarsAreVisible());
    auto [isVisible] = sendResult.takeReplyOr(true);
    return isVisible;
}

void WebChromeClientHaiku::setMenubarVisible(bool visible)
{
    m_page.send(Messages::WebPageProxy::SetMenubarVisible(visible));
}

bool WebChromeClientHaiku::menubarVisible() const
{
    auto sendResult = m_page.sendSync(Messages::WebPageProxy::MenubarIsVisible());
    auto [isVisible] = sendResult.takeReplyOr(true);
    return isVisible;
}

void WebChromeClientHaiku::setResizable(bool resizable)
{
    m_page.send(Messages::WebPageProxy::SetResizable(resizable));
}

void WebChromeClientHaiku::closeWindow()
{
    if (RefPtr coreFrame = m_page.mainFrame().coreLocalFrame())
        coreFrame->loader().stopForUserCancel();

    m_page.sendClose();
}

void WebChromeClientHaiku::addMessageToConsole(MessageSource source, MessageLevel level, const String& message, unsigned lineNumber, unsigned columnNumber, const String& sourceID)
{
    m_page.send(Messages::WebPageProxy::AddMessageToConsole(source, level, message, lineNumber, columnNumber, sourceID));
}

void WebChromeClientHaiku::mouseDidMoveOverElement(const HitTestResult& result, OptionSet<PlatformEventModifier> modifiers, const String&, TextDirection)
{
    m_page.send(Messages::WebPageProxy::MouseDidMoveOverElement(result, modifiers, {}));
}

void WebChromeClientHaiku::setCursor(const WebCore::Cursor& cursor)
{
    m_page.send(Messages::WebPageProxy::SetCursor(cursor));
}

void WebChromeClientHaiku::print(LocalFrame& frame)
{
    m_page.sendSync(Messages::WebPageProxy::PrintFrame(frame.frameID(), frame.document()->title(), {}));
}

void WebChromeClientHaiku::runOpenPanel(LocalFrame& frame, FileChooser& chooser)
{
    if (m_page.activeOpenPanelResultListener())
        return;

    Ref<WebOpenPanelResultListener> listener = WebOpenPanelResultListener::create(m_page, chooser);
    m_page.setActiveOpenPanelResultListener(listener.ptr());

    m_page.send(Messages::WebPageProxy::RunOpenPanel(frame.frameID(), frame.info(), chooser.settings()), listener->listenerID());
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

RefPtr<Page> WebChromeClientHaiku::createWindow(LocalFrame&, const String&, const WindowFeatures&, const NavigationAction&)
{
    // The decision to create a new window is handled in the WebFrameLoaderClient
    // by sending a policy decision request to the UI process.
    // This method can be a no-op.
    return nullptr;
}

void WebChromeClientHaiku::showContextMenu(const ContextMenu* menu) const
{
    Vector<WebContextMenuItemData> menuItems;
    for (const auto& item : menu->items())
        menuItems.append(WebContextMenuItemData(item));

    m_page.send(Messages::WebPageProxy::ShowContextMenu(
            m_page.contextMenuContextData(), menuItems));
}

} // namespace WebKit
