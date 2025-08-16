/*
 * Copyright (C) 2024 Haiku, Inc.
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
#include "DrawingAreaProxyHaiku.h"

#include "ShareableBitmap.h"
#include "UpdateInfo.h"
#include "WebPageProxy.h"
#include <WebCore/GraphicsContext.h>

#include <View.h>

namespace WebKit {
using namespace WebCore;

DrawingAreaProxyHaiku::DrawingAreaProxyHaiku(WebPageProxy& page, WebProcessProxy& process)
    : DrawingAreaProxy(DrawingAreaType::Haiku, page, process)
{
}

DrawingAreaProxyHaiku::~DrawingAreaProxyHaiku()
{
}

void DrawingAreaProxyHaiku::paint(GraphicsContext& context, const IntRect& rect)
{
    if (!m_backingStore)
        return;

    BView* view = context.platformContext();
    if (!view)
        return;

    view->DrawBitmap(m_backingStore.get(), rect, rect);
}

void DrawingAreaProxyHaiku::incorporateUpdate(const UpdateInfo& updateInfo)
{
    if (!m_backingStore)
        return;

    for (const auto& rect : updateInfo.updateRects()) {
        RefPtr<ShareableBitmap> bitmap = ShareableBitmap::create(updateInfo.bitmapHandle());
        if (!bitmap)
            return;

        const void* data = bitmap->data();
        if (!data)
            return;

        m_backingStore->ImportBits(data, bitmap->size().width() * bitmap->size().height() * 4,
            bitmap->size().width() * 4, 0, rect.location(), rect.size().width(), rect.size().height());
    }

    m_pageProxy.viewNeedsDisplay(updateInfo.updateRects());
}

void DrawingAreaProxyHaiku::sizeDidChange()
{
    m_backingStore = std::make_unique<BBitmap>(BRect(0, 0, size().width() - 1, size().height() - 1), B_RGB32, true);
}

} // namespace WebKit
