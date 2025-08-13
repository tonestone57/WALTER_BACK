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

#include "BWebView.h"

#include "BWebHistory.h"
#include "BWebSettings.h"
#include "WebBackForwardList.h"
#include "BWebPage.h"
#include "WebContext.h"
#include "WebPageGroup.h"
#include "WebPageProxy.h"
#include "WebView.h"
#include "WebEventFactory.h"
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>

#include <memory>

BWebView::BWebView(const char* name)
    : BView(name, B_WILL_DRAW | B_FRAME_EVENTS)
    , m_page(WebKit::WebProcessPool::create()-> createContext()->createWebPage(WebKit::WebPageGroup::create()))
{
    m_webView = std::make_unique<WebKit::WebView>(m_page);
    m_webPage = std::make_unique<BWebPage>(this);
    m_settings = new BWebSettings(*this);
    SetLayout(new BGroupLayout(B_HORIZONTAL));
    AddChild(BGroupLayoutBuilder(B_HORIZONTAL).Add(m_webView.get()));
}

BWebView::~BWebView()
{
}

BWebPage* BWebView::WebPage() const
{
    return m_webPage.get();
}

BReference<BWebSettings> BWebView::Settings() const
{
    return m_settings;
}

BReference<BWebHistory> BWebView::History()
{
    return new BWebHistory(m_page.backForwardList());
}

bool BWebView::CanGoBack()
{
    return m_webPage->CanGoBack();
}

bool BWebView::CanGoForward()
{
    return m_webPage->CanGoForward();
}

void BWebView::GoBack()
{
    m_webPage->GoBack();
}

void BWebView::GoForward()
{
    m_webPage->GoForward();
}

void BWebView::LoadURL(const char* url)
{
    m_page.loadUrl(WTF::String::fromUTF8(url));
}

void BWebView::Reload()
{
    m_page.reload();
}

void BWebView::StopLoading()
{
    m_page.stopLoading();
}

#include "PrintInfo.h"

void BWebView::Print(const BMessage* settings, float availablePaperWidth, float availablePaperHeight)
{
    WebKit::PrintInfo printInfo;
    printInfo.availablePaperWidth = availablePaperWidth;
    printInfo.availablePaperHeight = availablePaperHeight;
    if (settings)
        printInfo.printSettings = std::make_unique<BMessage>(*settings);

    m_page.print(printInfo, [](WebKit::CallbackBase::Error) {
        // FIXME: Handle completion
    });
}

void BWebView::GetContentsAsString(BFile& file)
{
    m_page.getContentsAsString([&file](const String& content, WebKit::CallbackBase::Error) {
        if (!content.isEmpty()) {
            const auto& utf8 = content.utf8();
            file.Write(utf8.data(), utf8.length());
        }
    });
}

void BWebView::ShowInspector()
{
    m_page.showInspector();
}

void BWebView::FindString(const char* string, bool forward,
    bool caseSensitive, bool wrapSelection, bool startInSelection)
{
    m_webPage->FindString(string, forward, caseSensitive, wrapSelection, startInSelection);
}

void BWebView::IncreaseZoomFactor(bool textOnly)
{
    m_webPage->IncreaseZoomFactor(textOnly);
}

void BWebView::DecreaseZoomFactor(bool textOnly)
{
    m_webPage->DecreaseZoomFactor(textOnly);
}

void BWebView::ResetZoomFactor()
{
    m_webPage->ResetZoomFactor();
}

void BWebView::SetDarkMode(bool dark)
{
    m_webPage->SetDarkMode(dark);
}

#include <Window.h>
#include <Message.h>

void BWebView::MouseDown(BPoint where)
{
    int32 buttons = 0;
    if (Window() && Window()->CurrentMessage())
        Window()->CurrentMessage()->FindInt32("buttons", &buttons);

    m_page.handleMouseEvent(WebKit::WebEventFactory::createWebMouseEvent(this, WebCore::WebEventType::MouseDown,
        where, buttons, 0));
}

void BWebView::MouseUp(BPoint where)
{
    int32 buttons = 0;
    if (Window() && Window()->CurrentMessage())
        Window()->CurrentMessage()->FindInt32("buttons", &buttons);

    m_page.handleMouseEvent(WebKit::WebEventFactory::createWebMouseEvent(this, WebCore::WebEventType::MouseUp,
        where, buttons, 0));
}

void BWebView::MouseMoved(BPoint where, uint32 transit, const BMessage* dragMessage)
{
    m_page.handleMouseEvent(WebKit::WebEventFactory::createWebMouseEvent(WebCore::WebEventType::MouseMove,
        where, where, 0, 0, 0));
    m_webPage->MouseMoved(where, transit, dragMessage);
}

void BWebView::MessageReceived(BMessage* message)
{
    m_webPage->MessageReceived(message);
    BView::MessageReceived(message);
}

void BWebView::KeyDown(const char* bytes, int32 numBytes)
{
    BMessage* message = Window()->CurrentMessage();
    m_page.handleKeyboardEvent(WebKit::WebEventFactory::createWebKeyboardEvent(message));
}
