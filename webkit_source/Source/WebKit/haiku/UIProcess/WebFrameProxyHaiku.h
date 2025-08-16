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

#pragma once

#include "DataReference.h"
#include "WebFrameProxy.h"

#include <Point.h>
#include <String.h>

#include <JavaScriptCore/JSExportMacros.h>

namespace WebKit {

class WebFrameProxyHaiku : public WebFrameProxy {
public:
    static Ref<WebFrameProxy> create(WebPageProxy&, WebCore::FrameIdentifier);
    virtual ~WebFrameProxyHaiku();

    void loadURL(const String&);
    void loadData(const IPC::DataReference&, const String&, const String&, const String&);
    void loadAlternateHTML(const IPC::DataReference&, const String&, const String&);
    void stopLoading();
    void reload();
    void goBack();
    void goForward();

    String requestedURL() const;
    String url() const;
    String mimeType() const;

    bool canCopy() const;
    bool canCut() const;
    bool canPaste() const;

    void copy();
    void cut();
    void paste();

    bool canUndo() const;
    bool canRedo() const;

    void undo();
    void redo();

    bool allowsScrolling() const;
    void setAllowsScrolling(bool);
    BPoint scrollPosition();

    String frameSource() const;
    void setFrameSource(const String&);

    void setTransparent(bool);
    bool isTransparent() const;

    String innerText() const;
    String asMarkup() const;
    String externalRepresentation() const;

    bool findString(const String&, uint32_t);

    bool canIncreaseZoomFactor() const;
    bool canDecreaseZoomFactor() const;

    void increaseZoomFactor(bool textOnly);
    void decreaseZoomFactor(bool textOnly);

    void resetZoomFactor();

    void setEditable(bool);
    bool isEditable() const;

    void setTitle(const String&);
    const String& title() const;

    const char* name() const;

    void runJavaScript(const String&);

    JSGlobalContextRef globalContext() const;

private:
    WebFrameProxyHaiku(WebPageProxy&, WebCore::FrameIdentifier);
};

} // namespace WebKit
