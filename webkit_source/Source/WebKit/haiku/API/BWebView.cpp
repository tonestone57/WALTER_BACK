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

#include "WebContext.h"
#include "WebPageGroup.h"
#include "WebPageProxy.h"
#include "WebView.h"

BWebView::BWebView(const char* name)
    : BView(name, B_WILL_DRAW | B_FRAME_EVENTS)
    , m_page(WebKit::WebProcessPool::create()-> createContext()->createWebPage(WebKit::WebPageGroup::create()))
{
    m_webView = std::make_unique<WebKit::WebView>(m_page);
    AddChild(m_webView.get());
}

BWebView::~BWebView()
{
}

void BWebView::LoadURL(const char* url)
{
    m_page.loadUrl(WTF::String::fromUTF8(url));
}

void BWebView::GoBack()
{
    m_page.goBack();
}

void BWebView::GoForward()
{
    m_page.goForward();
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
