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
#include "DrawingAreaHaiku.h"

#include "DrawingAreaProxyMessages.h"
#include "WebPage.h"
#include "WebPageInlines.h"
#include <WebCore/GraphicsContext.h>
#include <WebCore/NotImplemented.h>
#include <WebCore/ShareableBitmap.h>

namespace WebKit {

Ref<DrawingArea> DrawingArea::create(WebPage& webPage, const WebPageCreationParameters& parameters)
{
    return adoptRef(*new DrawingAreaHaiku(webPage, parameters));
}

DrawingAreaHaiku::DrawingAreaHaiku(WebPage& webPage, const WebPageCreationParameters& parameters)
    : DrawingArea(DrawingAreaType::Haiku, parameters.drawingAreaIdentifier, webPage)
    , m_displayTimer(RunLoop::main(), this, &DrawingAreaHaiku::displayTimerFired)
{
}

DrawingAreaHaiku::~DrawingAreaHaiku()
{
}

void DrawingAreaHaiku::setNeedsDisplay()
{
    setNeedsDisplayInRect(m_webPage->bounds());
}

void DrawingAreaHaiku::setNeedsDisplayInRect(const WebCore::IntRect& rect)
{
    m_dirtyRegion.unite(rect);
    scheduleDisplay();
}

void DrawingAreaHaiku::scroll(const WebCore::IntRect& scrollRect, const WebCore::IntSize& scrollDelta)
{
    send(Messages::DrawingAreaProxy::Scroll(scrollRect, scrollDelta));

    WebCore::Region exposedRegion;
    if (abs(scrollDelta.width()) >= scrollRect.width() || abs(scrollDelta.height()) >= scrollRect.height()) {
        exposedRegion = scrollRect;
    } else {
        WebCore::Region scrolledRegion = scrollRect;
        scrolledRegion.move(scrollDelta);
        exposedRegion = scrollRect;
        exposedRegion.subtract(scrolledRegion);
    }

    if (exposedRegion.isEmpty())
        return;

    for (const auto& rect : exposedRegion.rects())
        setNeedsDisplayInRect(rect);
}

void DrawingAreaHaiku::display()
{
    if (m_isWaitingForDidUpdate)
        return;

    // The layout may have put the page into accelerated compositing mode.
    // If the LayerTreeHost is in charge of displaying, we have nothing more to do.
    if (m_layerTreeHost)
        return;

    if (m_dirtyRegion.isEmpty())
        return;

    WebCore::IntRect bounds = m_dirtyRegion.bounds();
    float deviceScaleFactor = m_webPage->corePage()->deviceScaleFactor();

    WebCore::IntSize bitmapSize = bounds.size();
    bitmapSize.scale(deviceScaleFactor);
    auto bitmap = WebCore::ShareableBitmap::create({ bitmapSize });
    if (!bitmap)
        return;

    auto handle = bitmap->createHandle();
    if (!handle)
        return;

    auto graphicsContext = bitmap->createGraphicsContext();
    if (graphicsContext) {
        graphicsContext->applyDeviceScaleFactor(deviceScaleFactor);
        graphicsContext->translate(-bounds.x(), -bounds.y());
        for (const auto& rect : m_dirtyRegion.rects())
            m_webPage->drawRect(*graphicsContext, rect);
    }

    send(Messages::DrawingAreaProxy::Update(WTFMove(*handle), bounds));

    m_dirtyRegion.clear();
    m_isWaitingForDidUpdate = true;
}

void DrawingAreaHaiku::displayTimerFired()
{
    display();
}

void DrawingAreaHaiku::scheduleDisplay()
{
    if (m_displayTimer.isActive())
        return;
    m_displayTimer.startOneShot(0_s);
}

void DrawingAreaHaiku::didUpdate()
{
    m_isWaitingForDidUpdate = false;
    if (!m_dirtyRegion.isEmpty())
        scheduleDisplay();
}

void DrawingAreaHaiku::updateRenderingWithForcedRepaintAsync(WebPage&, CompletionHandler<void()>&& completionHandler)
{
    // For now, just trigger a display and hope for the best.
    // A proper implementation would wait for didUpdate.
    display();
    completionHandler();
}

#if USE(COORDINATED_GRAPHICS)
#include "LayerTreeHost.h"
#include <WebCore/PlatformScreen.h>
#include <app/Application.h>
#endif

void DrawingAreaHaiku::setRootCompositingLayer(WebCore::Frame&, WebCore::GraphicsLayer* rootLayer)
{
#if USE(COORDINATED_GRAPHICS)
    if (rootLayer) {
        if (!m_layerTreeHost) {
            m_layerTreeHost = makeUnique<LayerTreeHost>(m_webPage, be_app->InitialScreenID());
        }
        m_layerTreeHost->setRootCompositingLayer(rootLayer);
    } else {
        m_layerTreeHost = nullptr;
    }
#else
    notImplemented();
#endif
}

void DrawingAreaHaiku::triggerRenderingUpdate()
{
    scheduleDisplay();
}

} // namespace WebKit
