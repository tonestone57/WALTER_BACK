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
#include "WebChromeClient.h"

#include "WebPage.h"
#include <WebCore/WindowFeatures.h>

namespace WebKit {

using namespace WebCore;

void WebChromeClient::platformSetWindowRect(const FloatRect& rect)
{
    // Haiku-specific implementation for setting the window rectangle.
}

FloatRect WebChromeClient::platformGetWindowRect()
{
    // Haiku-specific implementation for getting the window rectangle.
    return FloatRect();
}

void WebChromeClient::platformFocus()
{
    // Haiku-specific implementation for focusing the window.
}

void WebChromeClient::platformUnfocus()
{
    // Haiku-specific implementation for unfocusing the window.
}

void WebChromeClient::platformRunJavaScriptAlert(WebFrame* frame, const String& message, Frame&, CompletionHandler<void()>&& completionHandler)
{
    // Haiku-specific implementation for running a JavaScript alert.
    completionHandler();
}

void WebChromeClient::platformRunJavaScriptConfirm(WebFrame* frame, const String& message, Frame&, CompletionHandler<void(bool)>&& completionHandler)
{
    // Haiku-specific implementation for running a JavaScript confirm.
    completionHandler(false);
}

void WebChromeClient::platformRunJavaScriptPrompt(WebFrame* frame, const String& message, const String& defaultValue, Frame&, CompletionHandler<void(const String&)>&& completionHandler)
{
    // Haiku-specific implementation for running a JavaScript prompt.
    completionHandler(String());
}

void WebChromeClient::platformSetStatusText(const String& statusText)
{
    // Haiku-specific implementation for setting the status text.
}

void WebChromeClient::platformDidCompletePageLoadForMainFrame()
{
    // Haiku-specific actions to take after the main frame has loaded.
}

void WebChromeClient::platformDidFinishLoading()
{
    // Haiku-specific actions to take after the page has finished loading.
}

void WebChromeClient::platformIsPlayingAudioDidChange(bool isPlayingAudio)
{
    // Haiku-specific handling of audio playback state changes.
}

RefPtr<WebCore::Page> WebChromeClient::platformCreateNewPage(WebCore::Page* page, const WebCore::WindowFeatures& windowFeatures, WebCore::Frame& frame, const WebCore::FrameLoadRequest& frameLoadRequest)
{
    // Haiku-specific implementation for creating a new page.
    return nullptr;
}

} // namespace WebKit
