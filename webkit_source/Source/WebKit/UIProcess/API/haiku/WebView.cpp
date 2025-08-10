/*
 * Copyright (C) 2009 Maxime Simon <simon.maxime@gmail.com>
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebView.h"

#include "NotImplemented.h"
#include "WebContext.h"
#include "WebPageGroup.h"
#include "WKPage.h"
#include "WKString.h"
#include "WKURL.h"
#include "WebPage.h"

#include <Window.h>

using namespace WebKit;


static const int32 kMsgNavigateArrow = '_NvA';


BWebView::UserData::~UserData()
{
}


BWebView::BWebView(const char* name, BPrivate::Network::BUrlContext* urlContext)
    : BView(name, B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE
	| B_NAVIGABLE | B_PULSE_NEEDED)
    , fLastMouseButtons(0)
    , fLastInputTime(-2000000)
    , fLastMousePos(0, 0)
    , fLastScrollOffset(0, 0)
    , fAutoHidePointer(false)
    , fUserData(nullptr)
{
    // FIXME: This is a hack. We should be getting the context and page group from the caller.
    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());
    WKRetainPtr<WKPageGroupRef> pageGroup = adoptWK(WKPageGroupCreateWithIdentifier(WKStringCreateWithUTF8CString("default")));
    fWebView = adoptWK(WKViewCreate(context.get(), pageGroup.get()));
}

BWebView::~BWebView()
{
	SetUserData(0);
}

void BWebView::Shutdown()
{
	if (Window())
		RemoveSelf();
    notImplemented();
}

// #pragma mark - BView hooks

void BWebView::AttachedToWindow()
{
    notImplemented();
}

void BWebView::DetachedFromWindow()
{
    notImplemented();
}

void BWebView::Show()
{
    BView::Show();
    notImplemented();
}

void BWebView::Hide()
{
    notImplemented();
    BView::Hide();
}

void BWebView::Draw(BRect rect)
{
    notImplemented();
}

void BWebView::FrameResized(float width, float height)
{
    notImplemented();
}

void BWebView::GetPreferredSize(float* width, float* height)
{
	if (width)
		*width = 100;
	if (height)
		*height = 100;
}

void BWebView::MessageReceived(BMessage* message)
{
    notImplemented();
}

void BWebView::MakeFocus(bool focused)
{
	BView::MakeFocus(focused);
    notImplemented();
}

void BWebView::WindowActivated(bool activated)
{
    notImplemented();
}

void BWebView::MouseMoved(BPoint where, uint32, const BMessage*)
{
    notImplemented();
}

void BWebView::MouseDown(BPoint where)
{
	MakeFocus(true);
    SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);
    notImplemented();
}

void BWebView::MouseUp(BPoint where)
{
    notImplemented();
}

void BWebView::KeyDown(const char*, int32)
{
    notImplemented();
}

void BWebView::KeyUp(const char*, int32)
{
    notImplemented();
}

void BWebView::Pulse()
{
    notImplemented();
}

// #pragma mark - public API

BString BWebView::MainFrameTitle() const
{
    notImplemented();
    return BString();
}

BString BWebView::MainFrameRequestedURL() const
{
    notImplemented();
    return BString();
}

BString BWebView::MainFrameURL() const
{
    notImplemented();
    return BString();
}

void BWebView::LoadURL(const char* urlString, bool aquireFocus)
{
    WKRetainPtr<WKURLRef> url = adoptWK(WKURLCreateWithUTF8CString(urlString));
    WKPageLoadURL(WKViewGetPage(fWebView.get()), url.get());
}

void BWebView::Reload()
{
    WKPageReload(WKViewGetPage(fWebView.get()));
}

void BWebView::GoBack()
{
    WKPageGoBack(WKViewGetPage(fWebView.get()));
}

void BWebView::GoForward()
{
    WKPageGoForward(WKViewGetPage(fWebView.get()));
}

void BWebView::StopLoading()
{
    WKPageStopLoading(WKViewGetPage(fWebView.get()));
}

void BWebView::IncreaseZoomFactor(bool textOnly)
{
    notImplemented();
}

void BWebView::DecreaseZoomFactor(bool textOnly)
{
    notImplemented();
}

void BWebView::ResetZoomFactor()
{
    notImplemented();
}

void BWebView::FindString(const char* string, bool forward ,
    bool caseSensitive, bool wrapSelection, bool startInSelection)
{
    notImplemented();
}

void BWebView::SetDarkMode(bool dark)
{
    notImplemented();
}

void BWebView::SetAutoHidePointer(bool doIt)
{
	fAutoHidePointer = doIt;
}

void BWebView::SetUserData(BWebView::UserData* userData)
{
	if (fUserData == userData)
		return;

	delete fUserData;
	fUserData = userData;
}

BWebView::UserData* BWebView::GetUserData() const
{
	return fUserData;
}


void BWebView::SetInspectorView(BWebView* inspector)
{
    fInspectorView = inspector;
}


BWebView* BWebView::GetInspectorView()
{
    return fInspectorView;
}


void BWebView::SetRootLayer(WebCore::GraphicsLayer* layer)
{
}

