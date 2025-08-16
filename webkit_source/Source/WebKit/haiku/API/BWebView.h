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

#ifndef B_WEB_VIEW_H
#define B_WEB_VIEW_H

#include <View.h>
#include <memory>

#include "BWebPage.h"
#include "BWebHistory.h"

class BMessage;

namespace WebKit {
class BWebHistory;
class BWebSettings;
class WebPageProxy;
class WebView;
}

class BWebView : public BView {
public:
    BWebView(const char* name);
    virtual ~BWebView();

    void LoadURL(const char* url);
    void Reload();
    void StopLoading();

    void Print(const BMessage* settings, float availablePaperWidth, float availablePaperHeight);
    void SaveContentsToFile(BFile& file);
    void ShowInspector();

    void FindString(const char* string, bool forward = true,
                            bool caseSensitive = false, bool wrapSelection = true,
                            bool startInSelection = false);

    void IncreaseZoomFactor(bool textOnly);
    void DecreaseZoomFactor(bool textOnly);
    void ResetZoomFactor();

    void SetDarkMode(bool dark);

    // History
    BReference<BWebHistory> History();
    bool CanGoBack();
    bool CanGoForward();
    void GoBack();
    void GoForward();

    virtual void MessageReceived(BMessage* message);
    virtual void FrameResized(float newWidth, float newHeight);
    virtual void AttachedToWindow();
    virtual void DetachedFromWindow();
    virtual void WindowActivated(bool active);
    virtual void Pulse();

    BString MainFrameTitle() const;
    BString MainFrameURL() const;

    BWebPage* WebPage() const;
    BReference<BWebSettings> Settings() const;

private:
    WebKit::WebPageProxy& page() const { return m_page; }

    std::unique_ptr<WebKit::WebView> m_webView;
    WebKit::WebPageProxy& m_page;
    std::unique_ptr<BWebPage> m_webPage;
    BReference<BWebSettings> m_settings;
};

#endif // B_WEB_VIEW_H
