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
#include "BackingStore.h"

#include <View.h>
#include <Bitmap.h>
#include <GraphicsContext.h>
#include <GraphicsContextHaiku.h>

namespace WebKit {

BackingStore::BackingStore(const WebCore::IntSize& size, float deviceScaleFactor)
    : m_size(size)
    , m_deviceScaleFactor(deviceScaleFactor)
{
    BRect bounds(0, 0, size.width() - 1, size.height() - 1);
    m_bitmap = new BBitmap(bounds, B_RGBA32);
}

void BackingStore::incorporateUpdate(ShareableBitmap::Handle&& handle, const WebCore::IntRect& rect)
{
    RefPtr<ShareableBitmap> updateBitmap = ShareableBitmap::create(WTFMove(handle));
    if (!updateBitmap)
        return;

    BBitmap* updateBBitmap = updateBitmap->haikuBitmap();
    if (!updateBBitmap)
        return;

    uint8* from = (uint8*)updateBBitmap->Bits();
    uint8* to = (uint8*)m_bitmap->Bits();
    int32 fromBpr = updateBBitmap->BytesPerRow();
    int32 toBpr = m_bitmap->BytesPerRow();

    for (int32 y = rect.y(); y < rect.maxY(); y++) {
        memcpy(to + y * toBpr + rect.x() * 4,
            from + y * fromBpr + rect.x() * 4,
            rect.width() * 4);
    }
}

void BackingStore::paint(BView* view, const WebCore::IntRect& rect)
{
    if (m_bitmap) {
        BRect dirty(rect.x(), rect.y(), rect.maxX() - 1, rect.maxY() - 1);
        view->DrawBitmap(m_bitmap, dirty, dirty);
    }
}

} // namespace WebKit
