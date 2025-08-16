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
#include "WebView.h"

#include "DrawingAreaProxyHaiku.h"
#include "NativeWebKeyboardEvent.h"
#include "NativeWebMouseEvent.h"
#include "WebPageProxy.h"
#include <WebCore/IntRect.h>
#include <Window.h>

namespace WebKit {

WebView::WebView(WebPageProxy& page)
    : BView("WebKit WebView", B_WILL_DRAW | B_FRAME_EVENTS)
    , m_page(page)
    , m_dragSource(page)
    , m_dragDestination(page)
{
}

void WebView::Draw(BRect updateRect)
{
    if (auto* drawingArea = static_cast<DrawingAreaProxyHaiku*>(m_page.drawingArea()))
        drawingArea->paint(this, WebCore::IntRect(updateRect));
}

void WebView::MouseDown(BPoint where)
{
    MakeFocus(true);
    BMessage* message = Window()->CurrentMessage();
    if (message)
        m_page.handleMouseEvent(NativeWebMouseEvent(message, this));
}

void WebView::MouseUp(BPoint where)
{
    BMessage* message = Window()->CurrentMessage();
    if (message)
        m_page.handleMouseEvent(NativeWebMouseEvent(message, this));
}

void WebView::MouseMoved(BPoint where, uint32 transit, const BMessage* dragMessage)
{
    if (dragMessage) {
        switch (transit) {
        case B_ENTERED_VIEW:
            m_dragDestination.DragEntered(dragMessage, where);
            break;
        case B_INSIDE_VIEW:
            m_dragDestination.DragUpdated(dragMessage, where);
            break;
        case B_EXITED_VIEW:
            m_dragDestination.DragExited(dragMessage, where);
            break;
        }
    } else {
        BMessage* message = Window()->CurrentMessage();
        if (message)
            m_page.handleMouseEvent(NativeWebMouseEvent(message, this));
    }
}

void WebView::MessageReceived(BMessage* message)
{
    switch (message->what) {
    case B_SIMPLE_DATA: {
        BPoint dropPoint;
        // The message should contain the drop point in view coordinates.
        if (message->FindPoint("be:drop_point", &dropPoint) != B_OK) {
            // As a fallback, get the mouse position at the time of the drop.
            uint32 buttons;
            GetMouse(&dropPoint, &buttons, false);
        }
        m_dragDestination.Drop(message, dropPoint);
        break;
    }
    default:
        BView::MessageReceived(message);
    }
}

void WebView::KeyDown(const char* bytes, int32 numBytes)
{
    BMessage* message = Window()->CurrentMessage();
    if (message)
        m_page.handleKeyboardEvent(NativeWebKeyboardEvent(message, this));
}

void WebView::KeyUp(const char* bytes, int32 numBytes)
{
    BMessage* message = Window()->CurrentMessage();
    if (message)
        m_page.handleKeyboardEvent(NativeWebKeyboardEvent(message, this));
}

} // namespace WebKit
