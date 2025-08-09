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

#pragma once

#include "WebChromeClient.h"

namespace WebKit {

class WebChromeClientHaiku final : public WebChromeClient {
public:
    WebChromeClientHaiku(WebPage&);

private:
    RefPtr<WebCore::Page> createWindow(WebCore::LocalFrame&, const String&, const WebCore::WindowFeatures&, const WebCore::NavigationAction&) final;
    void runJavaScriptAlert(WebCore::LocalFrame&, const String&) final;
    bool runJavaScriptConfirm(WebCore::LocalFrame&, const String&) final;
    bool runJavaScriptPrompt(WebCore::LocalFrame&, const String&, const String&, String&) final;
    void setToolbarsVisible(bool) final;
    void setStatusbarVisible(bool) final;
    void setScrollbarsVisible(bool) final;
    void setMenubarVisible(bool) final;
    void setResizable(bool) final;
    void addMessageToConsole(JSC::MessageSource, JSC::MessageLevel, const String&, unsigned, unsigned, const String&) final;
    void runOpenPanel(WebCore::LocalFrame&, WebCore::FileChooser&) final;
    void print(WebCore::LocalFrame&, const WebCore::StringWithDirection&) final;
};

} // namespace WebKit
