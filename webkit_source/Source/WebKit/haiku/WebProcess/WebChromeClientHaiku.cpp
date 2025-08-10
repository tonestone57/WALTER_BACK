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
#include "WebChromeClientHaiku.h"

#include "WebPage.h"
#include <WebCore/WindowFeatures.h>

namespace WebKit {

using namespace WebCore;

WebChromeClientHaiku::WebChromeClientHaiku(WebPage& page)
    : WebChromeClient(page)
{
}

void WebChromeClientHaiku::setWindowRect(const FloatRect& rect)
{
    // TODO: Implement
}

FloatRect WebChromeClientHaiku::windowRect() const
{
    // TODO: Implement
    return FloatRect();
}

void WebChromeClientHaiku::focus()
{
    // TODO: Implement
}

void WebChromeClientHaiku::unfocus()
{
    // TODO: Implement
}

void WebChromeClientHaiku::runJavaScriptAlert(LocalFrame&, const String&)
{
    // TODO: Implement
}

bool WebChromeClientHaiku::runJavaScriptConfirm(LocalFrame&, const String&)
{
    // TODO: Implement
    return false;
}

bool WebChromeClientHaiku::runJavaScriptPrompt(LocalFrame&, const String&, const String&, String&)
{
    // TODO: Implement
    return false;
}

void WebChromeClientHaiku::setStatusbarVisible(bool)
{
    // TODO: Implement
}

void WebChromeClientHaiku::setToolbarsVisible(bool)
{
    // TODO: Implement
}

void WebChromeClientHaiku::setMenubarVisible(bool)
{
    // TODO: Implement
}

void WebChromeClientHaiku::didCompletePageLoadForMainFrame()
{
    // TODO: Implement
}

void WebChromeClientHaiku::didFinishLoading()
{
    // TODO: Implement
}

void WebChromeClientHaiku::isPlayingAudioDidChange(bool)
{
    // TODO: Implement
}

RefPtr<Page> WebChromeClientHaiku::createWindow(LocalFrame&, const String&, const WindowFeatures&, const NavigationAction&)
{
    // TODO: Implement
    return nullptr;
}

} // namespace WebKit
