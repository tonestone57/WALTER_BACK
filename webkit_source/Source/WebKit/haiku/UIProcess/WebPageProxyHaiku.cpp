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
#include "WebPageProxy.h"

#include "APIView.h"
#include "DrawingAreaProxy.h"
#include "NativeWebMouseEvent.h"
#include "WebPageCreationParameters.h"
#include "WebPageProxyHaiku.h"
#include <WebCore/NotImplemented.h>

namespace WebKit {

void WebPageProxy::platformInitialize()
{
    // Haiku-specific initialization.
}

void WebPageProxy::getPlatformEditorState(WebCore::EditorState& state) const
{
    notImplemented();
}

Ref<API::View> WebPageProxy::createView()
{
    return API::View::create(*this);
}

void WebPageProxy::setView(API::View* view)
{
    m_view = view;
    if (m_view)
        m_view->setClient(makeUnique<ViewClient>());
}

void WebPageProxy::makeViewBlank()
{
    if (m_view)
        m_view->makeBlank();
}

void WebPageProxy::setDrawingArea(std::unique_ptr<DrawingAreaProxy>&& drawingArea)
{
    if (drawingArea)
        drawingArea->setShouldScaleViewToFitDocument(true);
    m_drawingArea = WTFMove(drawingArea);
}

} // namespace WebKit
