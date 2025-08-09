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
#include "WebChromeClientHaiku.h"

#include <wtf/TZoneMallocInlines.h>

namespace WebKit {

WebChromeClientHaiku::WebChromeClientHaiku(WebPage& page)
    : WebChromeClient(page)
{
}

RefPtr<WebCore::Page> WebChromeClientHaiku::createWindow(WebCore::LocalFrame&, const String&, const WebCore::WindowFeatures&, const WebCore::NavigationAction&)
{
    // FIXME: Implement. This should send a message to the UIProcess to create a new window.
    return nullptr;
}

void WebChromeClientHaiku::runJavaScriptAlert(WebCore::LocalFrame&, const String& message)
{
    // FIXME: Implement. This should send a message to the UIProcess to show an alert.
}

bool WebChromeClientHaiku::runJavaScriptConfirm(WebCore::LocalFrame&, const String& message)
{
    // FIXME: Implement. This should send a synchronous message to the UIProcess to show a confirm dialog.
    return false;
}

bool WebChromeClientHaiku::runJavaScriptPrompt(WebCore::LocalFrame&, const String&, const String&, String&)
{
    // FIXME: Implement. This should send a synchronous message to the UIProcess to show a prompt dialog.
    return false;
}

void WebChromeClientHaiku::setToolbarsVisible(bool)
{
    // FIXME: Implement. This should send a message to the UIProcess.
}

void WebChromeClientHaiku::setStatusbarVisible(bool)
{
    // FIXME: Implement. This should send a message to the UIProcess.
}

void WebChromeClientHaiku::setScrollbarsVisible(bool)
{
    // FIXME: Implement. This should send a message to the UIProcess.
}

void WebChromeClientHaiku::setMenubarVisible(bool)
{
    // FIXME: Implement. This should send a message to the UIProcess.
}

void WebChromeClientHaiku::setResizable(bool)
{
    // FIXME: Implement. This should send a message to the UIProcess.
}

void WebChromeClientHaiku::addMessageToConsole(JSC::MessageSource, JSC::MessageLevel, const String&, unsigned, unsigned, const String&)
{
    // FIXME: Implement. This should send a message to the UIProcess to log to the console.
}

void WebChromeClientHaiku::runOpenPanel(WebCore::LocalFrame&, WebCore::FileChooser&)
{
    // FIXME: Implement. This should send a message to the UIProcess to show a file chooser.
}

void WebChromeClientHaiku::print(WebCore::LocalFrame&, const WebCore::StringWithDirection&)
{
    // FIXME: Implement. This should send a message to the UIProcess to handle printing.
}

} // namespace WebKit
