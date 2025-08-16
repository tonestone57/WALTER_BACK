/*
 * Copyright (C) 2013 Intel Corporation. All rights reserved.
 * Copyright (C) 2024 Haiku, Inc.
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
#include "WebViewPrivate.h"

#include "DrawingAreaProxyHaiku.h"
#include "NotImplemented.h"
#include "WebContext.h"
#include "WebPageGroup.h"
#include "WebPageProxy.h"

#include <View.h>

using namespace WebCore;

namespace WebKit {

WebView::WebView(WebContext* context, WebPageGroup* pageGroup)
    : BView(nullptr, B_WILL_DRAW | B_FRAME_EVENTS)
{
    WebPageConfiguration webPageConfiguration;
    webPageConfiguration.pageGroup = pageGroup;

    m_page = context->createWebPage(*this, WTF::move(webPageConfiguration));

    m_page->pageGroup().preferences().setAcceleratedCompositingEnabled(true);
    m_page->pageGroup().preferences().setForceCompositingMode(true);

    char* debugVisualsEnvironment = getenv("WEBKIT_SHOW_COMPOSITING_DEBUG_VISUALS");
    bool showDebugVisuals = debugVisualsEnvironment && !strcmp(debugVisualsEnvironment, "1");
    m_page->pageGroup().preferences().setCompositingBordersVisible(showDebugVisuals);
    m_page->pageGroup().preferences().setCompositingRepaintCountersVisible(showDebugVisuals);
}

WebView::~WebView()
{
    if (m_page->isClosed())
        return;

    m_page->close();
}

PassRefPtr<WebView> WebView::create(WebContext* context, WebPageGroup* pageGroup)
{
    return adoptRef(new WebView(context, pageGroup));
}

void WebView::didChangeContentSize(WebCore::IntSize const&)
{
}

std::unique_ptr<WebKit::DrawingAreaProxy> WebView::createDrawingAreaProxy()
{
    return makeUnique<DrawingAreaProxyHaiku>(*m_page, m_page->process());
}

void WebView::setViewNeedsDisplay(const WebCore::IntRect& rect)
{
    Invalidate(rect);
}

void WebView::Draw(BRect updateRect)
{
    if (m_page && m_page->drawingArea())
        static_cast<DrawingAreaProxyHaiku*>(m_page->drawingArea())->paint(
            *GetGraphicsContext(), updateRect);
}

void WebView::MouseDown(BPoint where)
{
    if (m_page)
        m_page->handleMouseEvent(NativeWebMouseEvent(this, where, B_PRIMARY_MOUSE_BUTTON));
}

void WebView::MouseUp(BPoint where)
{
    if (m_page)
        m_page->handleMouseEvent(NativeWebMouseEvent(this, where, 0));
}

void WebView::MouseMoved(BPoint where, uint32 transit, const BMessage* message)
{
    if (m_page)
        m_page->handleMouseEvent(NativeWebMouseEvent(this, where, 0));
}

void WebView::KeyDown(const char* bytes, int32 numBytes)
{
    if (m_page)
        m_page->handleKeyboardEvent(NativeWebKeyboardEvent(this, bytes, numBytes));
}

void WebView::KeyUp(const char* bytes, int32 numBytes)
{
    if (m_page)
        m_page->handleKeyboardEvent(NativeWebKeyboardEvent(this, bytes, numBytes));
}

void WebView::scrollView(WebCore::IntRect const&, WebCore::IntSize const&)
{
}

void WebView::requestScroll(WebCore::FloatPoint const&, bool)
{
}

WebCore::IntSize WebView::viewSize()
{
    return IntSize(Bounds().IntegerWidth(), Bounds().IntegerHeight());
}

bool WebView::isViewWindowActive()
{
    return Window() && Window()->IsActive();
}

bool WebView::isViewFocused()
{
    return IsFocus();
}

bool WebView::isViewVisible()
{
    return !IsHidden();
}

void WebView::processDidExit()
{
}

void WebView::didRelaunchProcess()
{
}

void WebView::pageClosed()
{
}

void WebView::preferencesDidChange()
{
}

void WebView::toolTipChanged(WTF::String const&, WTF::String const&)
{
}

void WebView::didCommitLoadForMainFrame(WTF::String const&, bool)
{
}

void WebView::setCursor(WebCore::Cursor const&)
{
}

void WebView::setCursorHiddenUntilMouseMoves(bool)
{
}

void WebView::didChangeViewportProperties(WebCore::ViewportAttributes const&)
{
}

bool WebView::isViewInWindow()
{
    return Window();
}

void WebView::registerEditCommand(WTF::PassRefPtr<WebKit::WebEditCommandProxy>, WebKit::WebPageProxy::UndoOrRedo)
{
}

void WebView::clearAllEditCommands()
{
}

bool WebView::canUndoRedo(WebKit::WebPageProxy::UndoOrRedo)
{
    return false;
}

void WebView::executeUndoRedo(WebKit::WebPageProxy::UndoOrRedo)
{
}

WebCore::FloatRect WebView::convertToDeviceSpace(WebCore::FloatRect const& rect)
{
    return rect;
}

WebCore::IntPoint WebView::screenToRootView(WebCore::IntPoint const&)
{
    return { };
}

WebCore::IntRect WebView::rootViewToScreen(WebCore::IntRect const& rect)
{
    return rect;
}

WebCore::FloatRect WebView::convertToUserSpace(WebCore::FloatRect const& rect)
{
    return rect;
}

void WebView::updateTextInputState()
{
}

void WebView::handleDownloadRequest(WebKit::DownloadProxy*)
{
}

void WebView::doneWithKeyEvent(WebKit::NativeWebKeyboardEvent const&, bool)
{
}

WTF::PassRefPtr<WebKit::WebPopupMenuProxy> WebView::createPopupMenuProxy(WebKit::WebPageProxy*)
{
    return nullptr;
}

WTF::PassRefPtr<WebKit::WebContextMenuProxy> WebView::createContextMenuProxy(WebKit::WebPageProxy*)
{
    return nullptr;
}

void WebView::setFindIndicator(WTF::PassRefPtr<WebKit::FindIndicator>, bool, bool)
{
}

void WebView::enterAcceleratedCompositingMode(WebKit::LayerTreeContext const&)
{
}

void WebView::exitAcceleratedCompositingMode()
{
}

void WebView::updateAcceleratedCompositingMode(WebKit::LayerTreeContext const&)
{
}

void WebView::didFinishLoadingDataForCustomContentProvider(WTF::String const&, IPC::DataReference const&)
{
}

void WebView::didFinishLoadForMainFrame()
{
}

void WebView::didFirstVisuallyNonEmptyLayoutForMainFrame()
{
}


}
