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

#ifndef B_WEB_WINDOW_H
#define B_WEB_WINDOW_H

#include <Window.h>

class BBitmap;
class BWebView;

class BWebWindow : public BWindow {
public:
    BWebWindow(BRect frame, const char* title, window_look look,
        window_feel feel, uint32 flags,
        uint32 workspace = B_CURRENT_WORKSPACE);
    virtual ~BWebWindow();

    BWebView* CurrentWebView() const { return m_currentWebView; }
    void SetCurrentWebView(BWebView* view) { m_currentWebView = view; }

    virtual void NavigationRequested(const BString& url, BWebView* view);
    virtual void NewWindowRequested(const BString& url, bool primaryAction);
    virtual void CloseWindowRequested(BWebView* view);
    virtual void NewPageCreated(BWebView* view, BRect windowFrame, bool modalDialog, bool resizable, bool activate);
    virtual void LoadNegotiating(const BString& url, BWebView* view);
    virtual void LoadCommitted(const BString& url, BWebView* view);
    virtual void LoadFailed(const BString& url, BWebView* view);
    virtual void LoadFinished(const BString& url, BWebView* view);
    virtual void LoadProgress(float progress, BWebView* view);
    virtual void MainDocumentError(const BString& failingURL, const BString& localizedDescription, BWebView* view);
    virtual void TitleChanged(const BString& title, BWebView* view);
    virtual void IconReceived(const BBitmap* icon, BWebView* view);
    virtual void ResizeRequested(float width, float height, BWebView* view);
    virtual void SetToolBarsVisible(bool flag, BWebView* view);
    virtual void SetStatusBarVisible(bool flag, BWebView* view);
    virtual void SetMenuBarVisible(bool flag, BWebView* view);
    virtual void SetResizable(bool flag, BWebView* view);
    virtual void StatusChanged(const BString& status, BWebView* view);
    virtual void NavigationCapabilitiesChanged(bool canGoBackward,
        bool canGoForward, bool canStop, BWebView* view);
    virtual void UpdateGlobalHistory(const BString& url);
    virtual bool AuthenticationChallenge(BString message,
                        BString& inOutUser, BString& inOutPassword,
                        bool& inOutRememberCredentials,
                        uint32 failureCount, BWebView* view);

private:
    BWebView* m_currentWebView;
};

#endif // B_WEB_WINDOW_H
