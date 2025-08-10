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
#include "WebFrameProxyHaiku.h"

#include <WebCore/NotImplemented.h>

namespace WebKit {

Ref<WebFrameProxy> WebFrameProxyHaiku::create(WebPageProxy& page, WebCore::FrameIdentifier frameID)
{
    return adoptRef(*new WebFrameProxyHaiku(page, frameID));
}

WebFrameProxyHaiku::WebFrameProxyHaiku(WebPageProxy& page, WebCore::FrameIdentifier frameID)
    : WebFrameProxy(page, frameID)
{
}

WebFrameProxyHaiku::~WebFrameProxyHaiku()
{
}

void WebFrameProxyHaiku::loadURL(const String& url)
{
    notImplemented();
}

void WebFrameProxyHaiku::stopLoading()
{
    notImplemented();
}

void WebFrameProxyHaiku::reload()
{
    notImplemented();
}

String WebFrameProxyHaiku::requestedURL() const
{
    notImplemented();
    return String();
}

String WebFrameProxyHaiku::url() const
{
    notImplemented();
    return String();
}

String WebFrameProxyHaiku::mimeType() const
{
    notImplemented();
    return String();
}

bool WebFrameProxyHaiku::canCopy() const
{
    notImplemented();
    return false;
}

bool WebFrameProxyHaiku::canCut() const
{
    notImplemented();
    return false;
}

bool WebFrameProxyHaiku::canPaste() const
{
    notImplemented();
    return false;
}

void WebFrameProxyHaiku::copy()
{
    notImplemented();
}

void WebFrameProxyHaiku::cut()
{
    notImplemented();
}

void WebFrameProxyHaiku::paste()
{
    notImplemented();
}

bool WebFrameProxyHaiku::canUndo() const
{
    notImplemented();
    return false;
}

bool WebFrameProxyHaiku::canRedo() const
{
    notImplemented();
    return false;
}

void WebFrameProxyHaiku::undo()
{
    notImplemented();
}

void WebFrameProxyHaiku::redo()
{
    notImplemented();
}

bool WebFrameProxyHaiku::allowsScrolling() const
{
    notImplemented();
    return false;
}

void WebFrameProxyHaiku::setAllowsScrolling(bool)
{
    notImplemented();
}

BPoint WebFrameProxyHaiku::scrollPosition()
{
    notImplemented();
    return BPoint();
}

String WebFrameProxyHaiku::frameSource() const
{
    notImplemented();
    return String();
}

void WebFrameProxyHaiku::setFrameSource(const String&)
{
    notImplemented();
}

void WebFrameProxyHaiku::setTransparent(bool)
{
    notImplemented();
}

bool WebFrameProxyHaiku::isTransparent() const
{
    notImplemented();
    return false;
}

String WebFrameProxyHaiku::innerText() const
{
    notImplemented();
    return String();
}

String WebFrameProxyHaiku::asMarkup() const
{
    notImplemented();
    return String();
}

String WebFrameProxyHaiku::externalRepresentation() const
{
    notImplemented();
    return String();
}

bool WebFrameProxyHaiku::findString(const String&, uint32_t)
{
    notImplemented();
    return false;
}

bool WebFrameProxyHaiku::canIncreaseZoomFactor() const
{
    notImplemented();
    return false;
}

bool WebFrameProxyHaiku::canDecreaseZoomFactor() const
{
    notImplemented();
    return false;
}

void WebFrameProxyHaiku::increaseZoomFactor(bool)
{
    notImplemented();
}

void WebFrameProxyHaiku::decreaseZoomFactor(bool)
{
    notImplemented();
}

void WebFrameProxyHaiku::resetZoomFactor()
{
    notImplemented();
}

void WebFrameProxyHaiku::setEditable(bool)
{
    notImplemented();
}

bool WebFrameProxyHaiku::isEditable() const
{
    notImplemented();
    return false;
}

void WebFrameProxyHaiku::setTitle(const String&)
{
    notImplemented();
}

const String& WebFrameProxyHaiku::title() const
{
    notImplemented();
    static String title;
    return title;
}

const char* WebFrameProxyHaiku::name() const
{
    notImplemented();
    return nullptr;
}

JSGlobalContextRef WebFrameProxyHaiku::globalContext() const
{
    notImplemented();
    return nullptr;
}

} // namespace WebKit
