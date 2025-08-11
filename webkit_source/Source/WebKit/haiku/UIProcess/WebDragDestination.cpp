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
#include "WebDragDestination.h"

#include "WebPageProxy.h"
#include "WebPageProxyHaiku.h"
#include <Message.h>
#include <View.h>
#include <WebCore/DragData.h>

namespace WebKit {

WebDragDestination::WebDragDestination(WebPageProxy& page)
    : m_page(page)
{
}

static WebCore::DragData dragDataFromBMessage(const BMessage* message, const BPoint& viewPoint, WebPageProxy& page)
{
    BPoint screenPoint = static_cast<WebPageProxyHaiku&>(page).view()->ConvertToScreen(viewPoint);
    WebCore::DragData dragData(nullptr, viewPoint, screenPoint, WebCore::DragOperation::None, {});

    if (message->HasString("text/uri-list")) {
        dragData.asURLData() = message->GetString("text/uri-list", "");
    } else if (message->HasString("text/plain")) {
        dragData.asTextData() = message->GetString("text/plain", "");
    }
    return dragData;
}

void WebDragDestination::DragEntered(const BMessage* message, const BPoint& point)
{
    m_page.performDragControllerAction(WebCore::DragControllerAction::Entered, dragDataFromBMessage(message, point, m_page));
}

void WebDragDestination::DragUpdated(const BMessage* message, const BPoint& point)
{
    m_page.performDragControllerAction(WebCore::DragControllerAction::Updated, dragDataFromBMessage(message, point, m_page));
}

void WebDragDestination::DragExited(const BMessage* message, const BPoint& point)
{
    m_page.performDragControllerAction(WebCore::DragControllerAction::Exited, dragDataFromBMessage(message, point, m_page));
}

void WebDragDestination::Drop(const BMessage* message, const BPoint& point)
{
    m_page.performDragControllerAction(WebCore::DragControllerAction::PerformDragOperation, dragDataFromBMessage(message, point, m_page));
}

} // namespace WebKit
