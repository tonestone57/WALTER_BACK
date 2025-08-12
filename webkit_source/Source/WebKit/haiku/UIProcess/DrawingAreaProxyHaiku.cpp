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
#include "DrawingAreaProxyHaiku.h"

#include "WebPageProxy.h"
#include <WebCore/NotImplemented.h>

namespace WebKit {

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

void DrawingAreaProxyHaiku::sizeDidChange()
{
    m_backingStore = std::make_unique<BackingStore>(size(), page()->deviceScaleFactor());
}

void DrawingAreaProxyHaiku::deviceScaleFactorDidChange(CompletionHandler<void()>&& completionHandler)
{
    m_backingStore = std::make_unique<BackingStore>(size(), page()->deviceScaleFactor());
    completionHandler();
}

void DrawingAreaProxyHaiku::paint(BView* view, const WebCore::IntRect& rect)
{
    if (m_backingStore)
        m_backingStore->paint(view, rect);
}

void DrawingAreaProxyHaiku::update(const ShareableBitmap::Handle& handle, const WebCore::IntRect& rect)
{
    if (!m_backingStore)
        m_backingStore = std::make_unique<BackingStore>(size(), page()->deviceScaleFactor());

    m_backingStore->incorporateUpdate(ShareableBitmap::Handle(handle), rect);

    if (auto* page = page())
        page->setViewNeedsDisplay(rect);
}

} // namespace WebKit
