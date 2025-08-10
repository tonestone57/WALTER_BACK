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
 * ARISING IN ANY WAY OUT of THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "DrawingAreaProxyHaiku.h"

#include "UpdateInfo.h"
#include "WebPageProxy.h"
#include <WebCore/NotImplemented.h>
#include <WebCore/ShareableBitmap.h>

namespace WebKit {

class DrawingAreaProxyHaiku::BackingStore {
public:
    BackingStore(const WebCore::IntSize& size, float deviceScaleFactor)
        : m_size(size)
        , m_deviceScaleFactor(deviceScaleFactor)
    {
    }

    void incorporateUpdate(UpdateInfo& updateInfo)
    {
        // For now, just replace the whole bitmap.
        m_bitmap = WebCore::ShareableBitmap::create(updateInfo.bitmapHandle);
    }

    void paint(BView* view, const WebCore::IntRect& rect)
    {
        if (m_bitmap)
            view->DrawBitmap(static_cast<BBitmap*>(m_bitmap->nativeImage()), rect);
    }

private:
    WebCore::IntSize m_size;
    float m_deviceScaleFactor;
    RefPtr<WebCore::ShareableBitmap> m_bitmap;
};

Ref<DrawingAreaProxy> DrawingAreaProxy::create(WebPageProxy& webPage, WebProcessProxy& webProcessProxy)
{
    return adoptRef(*new DrawingAreaProxyHaiku(webPage, webProcessProxy));
}

DrawingAreaProxyHaiku::DrawingAreaProxyHaiku(WebPageProxy& webPage, WebProcessProxy& webProcessProxy)
    : DrawingAreaProxy(DrawingAreaType::Haiku, webPage, webProcessProxy)
{
}

DrawingAreaProxyHaiku::~DrawingAreaProxyHaiku()
{
}

void DrawingAreaProxyHaiku::paint(BView* view, const WebCore::IntRect& rect)
{
    if (m_backingStore)
        m_backingStore->paint(view, rect);
}

void DrawingAreaProxyHaiku::sizeDidChange()
{
    // This method is called when the size of the view changes.
    // We will need to inform the WebProcess of the new size.
    notImplemented();
}

void DrawingAreaProxyHaiku::update(uint64_t, UpdateInfo&& updateInfo)
{
    if (!m_backingStore || m_backingStore->size() != updateInfo.viewSize || m_backingStore->deviceScaleFactor() != updateInfo.deviceScaleFactor)
        m_backingStore = makeUnique<BackingStore>(updateInfo.viewSize, updateInfo.deviceScaleFactor);

    m_backingStore->incorporateUpdate(updateInfo);

    if (auto* page = page())
        page->setViewNeedsDisplay(updateInfo.updateRectBounds);
}

} // namespace WebKit
