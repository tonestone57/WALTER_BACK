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
#include "WebDragSource.h"

#include "WebPageProxy.h"
#include "WebPageProxyHaiku.h"
#include <Bitmap.h>
#include <Message.h>
#include <View.h>
#include <WebCore/DragData.h>

namespace WebKit {

WebDragSource::WebDragSource(WebPageProxy& page)
    : m_page(page)
{
}

void WebDragSource::StartDrag(WebCore::DragData& dragData, BBitmap* dragImage)
{
    BMessage dragMessage(B_SIMPLE_DATA);
    if (dragData.asURLData()) {
        dragMessage.AddString("text/uri-list", dragData.asURL());
        dragMessage.AddString("text/plain", dragData.asURL());
    }
    if (dragData.asTextData())
        dragMessage.AddString("text/plain", dragData.asText());

    static_cast<WebPageProxyHaiku&>(m_page).view()->DragMessage(&dragMessage, dragImage, B_OP_ALPHA, BPoint(0, 0));
}

} // namespace WebKit
