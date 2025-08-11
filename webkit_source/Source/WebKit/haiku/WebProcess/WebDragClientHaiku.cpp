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
#include "WebDragClientHaiku.h"

#include "WebPage.h"

namespace WebKit {

WebDragClientHaiku::WebDragClientHaiku(WebPage& page)
    : m_page(page)
{
}

void WebDragClientHaiku::willPerformDragDestinationAction(WebCore::DragDestinationAction, WebCore::DragData&)
{
}

void WebDragClientHaiku::willPerformDragSourceAction(WebCore::DragSourceAction, const WebCore::IntPoint&, WebCore::DataTransfer&)
{
}

OptionSet<WebCore::DragSourceAction> WebDragClientHaiku::dragSourceActionMaskForPoint(const WebCore::IntPoint&)
{
    return WebCore::DragSourceAction::None;
}

void WebDragClientHaiku::startDrag(WebCore::DragImageRef, const WebCore::IntPoint&, const WebCore::IntPoint&, WebCore::DataTransfer&, WebCore::Frame&, bool)
{
}

void WebDragClientHaiku::dragControllerDestroyed()
{
    delete this;
}

} // namespace WebKit
