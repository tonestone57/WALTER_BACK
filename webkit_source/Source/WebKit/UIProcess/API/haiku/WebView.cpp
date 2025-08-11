/*
 * Copyright (C) 2024 Your Name <your@email.com>
 *
 * All rights reserved.
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
#include "WebView.h"

#include "DrawingAreaProxyHaiku.h"
#include "NativeWebKeyboardEvent.h"
#include "NativeWebMouseEvent.h"
#include "WebEventFactory.h"
#include "WebContextMenuProxyHaiku.h"
#include "WebPageProxy.h"
#include "WebProcessPool.h"

#include <Window.h>

namespace WebKit {

WebView::WebView(BRect frame, const char* name, uint32 resizingMode, uint32 flags)
    : BView(frame, name, resizingMode, flags)
{
    m_page = WebProcessPool::create("org.haiku.WebKit.WebContent")->createNewPage(nullptr, WebPageConfiguration::create());
}

WebView::~WebView()
{
}

void WebView::load(const char* url)
{
    m_page->loadURL(URL(String::fromUTF8(url)));
}

void WebView::reload()
{
    m_page->reload();
}

void WebView::goBack()
{
    m_page->goBack();
}

void WebView::goForward()
{
    m_page->goForward();
}

void WebView::stopLoading()
{
    m_page->stopLoading();
}

void WebView::Draw(BRect updateRect)
{
    DrawingAreaProxyHaiku* drawingArea = static_cast<DrawingAreaProxyHaiku*>(m_page->drawingArea());
    if (drawingArea && drawingArea->backingStore())
        DrawBitmap(drawingArea->backingStore(), updateRect, updateRect);
}

void WebView::MouseDown(BPoint where)
{
    BMessage* message = Window()->CurrentMessage();
    if (!message)
        return;

    m_page->handleMouseEvent(WebEventFactory::createWebMouseEvent(message, where, where).value());
}

void WebView::MouseUp(BPoint where)
{
    BMessage* message = Window()->CurrentMessage();
    if (!message)
        return;

    m_page->handleMouseEvent(WebEventFactory::createWebMouseEvent(message, where, where).value());
}

void WebView::MouseMoved(BPoint where, uint32 transit, const BMessage* dragMessage)
{
    BMessage* message = Window()->CurrentMessage();
    if (!message)
        return;

    m_page->handleMouseEvent(WebEventFactory::createWebMouseEvent(message, where, where).value());
}

void WebView::KeyDown(const char* bytes, int32 numBytes)
{
    BMessage* message = Window()->CurrentMessage();
    if (!message)
        return;

    m_page->handleKeyboardEvent(WebEventFactory::createWebKeyboardEvent(message).value());
}

void WebView::KeyUp(const char* bytes, int32 numBytes)
{
    BMessage* message = Window()->CurrentMessage();
    if (!message)
        return;

    m_page->handleKeyboardEvent(WebEventFactory::createWebKeyboardEvent(message).value());
}

Ref<WebContextMenuProxy> WebView::createContextMenuProxy(WebPageProxy& page, FrameInfoData&& frameInfo, ContextMenuContextData&& context, const UserData& userData)
{
    return WebContextMenuProxyHaiku::create(this, page, WTFMove(frameInfo), WTFMove(context), userData);
}

} // namespace WebKit
