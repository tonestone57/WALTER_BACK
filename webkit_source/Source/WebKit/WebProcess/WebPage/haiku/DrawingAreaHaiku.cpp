/*
 * Copyright (C) 2024 Your Name <you@example.com>
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
#include "DrawingAreaHaiku.h"

#include <wtf/TZoneMallocInlines.h>

namespace WebKit {

DrawingAreaHaiku::DrawingAreaHaiku(WebPage& webPage, const WebPageCreationParameters& parameters)
    : DrawingArea(DrawingAreaType::Tiled, parameters.drawingAreaIdentifier, webPage)
{
}

DrawingAreaHaiku::~DrawingAreaHaiku()
{
}

void DrawingAreaHaiku::setNeedsDisplay()
{
    // FIXME: Implement.
}

void DrawingAreaHaiku::setNeedsDisplayInRect(const WebCore::IntRect&)
{
    // FIXME: Implement.
}

void DrawingAreaHaiku::scroll(const WebCore::IntRect&, const WebCore::IntSize&)
{
    // FIXME: Implement.
}

void DrawingAreaHaiku::updateRenderingWithForcedRepaintAsync(WebPage&, CompletionHandler<void()>&& completionHandler)
{
    // FIXME: Implement.
    completionHandler();
}

void DrawingAreaHaiku::setRootCompositingLayer(WebCore::Frame&, WebCore::GraphicsLayer*)
{
    // FIXME: Implement.
}

void DrawingAreaHaiku::triggerRenderingUpdate()
{
    // FIXME: Implement.
}

} // namespace WebKit
