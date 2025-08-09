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
#include "PageClientImpl.h"

#include "DrawingAreaProxyCoordinatedGraphics.h"
#include "WebPageProxy.h"
#include <WebCore/NotImplemented.h>
#include <wtf/TZoneMallocInlines.h>

namespace WebKit {
using namespace WebCore;

WTF_MAKE_TZONE_ALLOCATED_IMPL(PageClientImpl);

PageClientImpl::PageClientImpl(BView* viewWidget)
    : m_viewWidget(viewWidget)
{
}

PageClientImpl::~PageClientImpl()
{
}

UniqueRef<API::NavigationClient> PageClientImpl::navigationClient()
{
    return makeUniqueRef<HaikuNavigationClient>();
}

Ref<DrawingAreaProxy> PageClientImpl::createDrawingAreaProxy(WebProcessProxy& webProcessProxy)
{
    // FIXME: Implement this.
    notImplemented();
    return DrawingAreaProxyCoordinatedGraphics::create(WebPageProxy::create(*this, webProcessProxy, API::PageConfiguration::create()), webProcessProxy);
}

void PageClientImpl::setViewNeedsDisplay(const WebCore::Region&)
{
    notImplemented();
}

void PageClientImpl::requestScroll(const WebCore::FloatPoint&, const WebCore::IntPoint&, WebCore::ScrollIsAnimated)
{
    notImplemented();
}

WebCore::FloatPoint PageClientImpl::viewScrollPosition()
{
    return { };
}

WebCore::IntSize PageClientImpl::viewSize()
{
    return { };
}

bool PageClientImpl::isViewWindowActive()
{
    return false;
}

bool PageClientImpl::isViewFocused()
{
    return false;
}

bool PageClientImpl::isViewVisible()
{
    return false;
}

bool PageClientImpl::isViewInWindow()
{
    return false;
}

void PageClientImpl::processDidExit()
{
    notImplemented();
}

void PageClientImpl::didRelaunchProcess()
{
    notImplemented();
}

void PageClientImpl::toolTipChanged(const String&, const String&)
{
    notImplemented();
}

void PageClientImpl::setCursor(const WebCore::Cursor&)
{
    notImplemented();
}

void PageClientImpl::setCursorHiddenUntilMouseMoves(bool)
{
    notImplemented();
}

void PageClientImpl::registerEditCommand(Ref<WebEditCommandProxy>&&, UndoOrRedo)
{
    notImplemented();
}

void PageClientImpl::clearAllEditCommands()
{
    notImplemented();
}

bool PageClientImpl::canUndoRedo(UndoOrRedo)
{
    return false;
}

void PageClientImpl::executeUndoRedo(UndoOrRedo)
{
    notImplemented();
}

WebCore::FloatRect PageClientImpl::convertToDeviceSpace(const WebCore::FloatRect& rect)
{
    return rect;
}

WebCore::FloatRect PageClientImpl::convertToUserSpace(const WebCore::FloatRect& rect)
{
    return rect;
}

WebCore::IntPoint PageClientImpl::screenToRootView(const WebCore::IntPoint& point)
{
    return point;
}

WebCore::IntPoint PageClientImpl::rootViewToScreen(const WebCore::IntPoint& point)
{
    return point;
}

WebCore::IntRect PageClientImpl::rootViewToScreen(const WebCore::IntRect& rect)
{
    return rect;
}

void PageClientImpl::doneWithKeyEvent(const NativeWebKeyboardEvent&, bool)
{
    notImplemented();
}

RefPtr<WebPopupMenuProxy> PageClientImpl::createPopupMenuProxy(WebPageProxy&)
{
    return nullptr;
}

Ref<WebContextMenuProxy> PageClientImpl::createContextMenuProxy(WebPageProxy&, FrameInfoData&&, ContextMenuContextData&&, const UserData&)
{
    notImplemented();
    return WebContextMenuProxyGtk::create(m_viewWidget, page, WTFMove(frameInfoData), WTFMove(context), userData);
}

RefPtr<WebColorPicker> PageClientImpl::createColorPicker(WebPageProxy&, const WebCore::Color&, const WebCore::IntRect&, ColorControlSupportsAlpha, Vector<WebCore::Color>&&)
{
    return nullptr;
}

RefPtr<WebDateTimePicker> PageClientImpl::createDateTimePicker(WebPageProxy&)
{
    return nullptr;
}

RefPtr<WebDataListSuggestionsDropdown> PageClientImpl::createDataListSuggestionsDropdown(WebPageProxy&)
{
    return nullptr;
}

void PageClientImpl::enterAcceleratedCompositingMode(const LayerTreeContext&)
{
    notImplemented();
}

void PageClientImpl::exitAcceleratedCompositingMode()
{
    notImplemented();
}

void PageClientImpl::updateAcceleratedCompositingMode(const LayerTreeContext&)
{
    notImplemented();
}

void PageClientImpl::pageClosed()
{
    notImplemented();
}

void PageClientImpl::preferencesDidChange()
{
    notImplemented();
}

void PageClientImpl::didChangeContentSize(const WebCore::IntSize&)
{
    notImplemented();
}

void PageClientImpl::wheelEventWasNotHandledByWebCore(const NativeWebWheelEvent&)
{
    notImplemented();
}

void PageClientImpl::didFinishLoadingDataForCustomContentProvider(const String&, std::span<const uint8_t>)
{
    notImplemented();
}

void PageClientImpl::navigationGestureDidBegin()
{
    notImplemented();
}

void PageClientImpl::navigationGestureWillEnd(bool, WebBackForwardListItem&)
{
    notImplemented();
}

void PageClientImpl::navigationGestureDidEnd(bool, WebBackForwardListItem&)
{
    notImplemented();
}

void PageClientImpl::navigationGestureDidEnd()
{
    notImplemented();
}

void PageClientImpl::willRecordNavigationSnapshot(WebBackForwardListItem&)
{
    notImplemented();
}

void PageClientImpl::didRemoveNavigationGestureSnapshot()
{
    notImplemented();
}

void PageClientImpl::didFirstVisuallyNonEmptyLayoutForMainFrame()
{
    notImplemented();
}

void PageClientImpl::didFinishNavigation(API::Navigation*)
{
    notImplemented();
}

void PageClientImpl::didFailNavigation(API::Navigation*)
{
    notImplemented();
}

void PageClientImpl::didSameDocumentNavigationForMainFrame(SameDocumentNavigationType)
{
    notImplemented();
}

void PageClientImpl::didChangeBackgroundColor()
{
    notImplemented();
}

void PageClientImpl::isPlayingAudioWillChange()
{
    notImplemented();
}

void PageClientImpl::isPlayingAudioDidChange()
{
    notImplemented();
}

void PageClientImpl::refView()
{
}

void PageClientImpl::derefView()
{
}

} // namespace WebKit
