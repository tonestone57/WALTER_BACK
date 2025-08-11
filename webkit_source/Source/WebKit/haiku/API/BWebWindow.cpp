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

#include "BWebWindow.h"

BWebWindow::BWebWindow(BRect frame, const char* title, window_look look,
    window_feel feel, uint32 flags, uint32 workspace)
    : BWindow(frame, title, look, feel, flags, workspace)
    , m_currentWebView(nullptr)
{
}

BWebWindow::~BWebWindow()
{
}

void BWebWindow::NavigationRequested(const BString& url, BWebView* view) { }
void BWebWindow::NewWindowRequested(const BString& url, bool primaryAction) { }
void BWebWindow::CloseWindowRequested(BWebView* view) { }
void BWebWindow::NewPageCreated(BWebView* view, BRect windowFrame, bool modalDialog, bool resizable, bool activate) { }
void BWebWindow::LoadNegotiating(const BString& url, BWebView* view) { }
void BWebWindow::LoadCommitted(const BString& url, BWebView* view) { }
void BWebWindow::LoadFailed(const BString& url, BWebView* view) { }
void BWebWindow::LoadFinished(const BString& url, BWebView* view) { }
void BWebWindow::LoadProgress(float progress, BWebView* view) { }
void BWebWindow::MainDocumentError(const BString& failingURL, const BString& localizedDescription, BWebView* view) { }
void BWebWindow::TitleChanged(const BString& title, BWebView* view) { }
void BWebWindow::IconReceived(const BBitmap* icon, BWebView* view) { }
void BWebWindow::ResizeRequested(float width, float height, BWebView* view) { }
void BWebWindow::SetToolBarsVisible(bool flag, BWebView* view) { }
void BWebWindow::SetStatusBarVisible(bool flag, BWebView* view) { }
void BWebWindow::SetMenuBarVisible(bool flag, BWebView* view) { }
void BWebWindow::SetResizable(bool flag, BWebView* view) { }
void BWebWindow::StatusChanged(const BString& status, BWebView* view) { }
void BWebWindow::NavigationCapabilitiesChanged(bool canGoBackward,
    bool canGoForward, bool canStop, BWebView* view) { }
void BWebWindow::UpdateGlobalHistory(const BString& url) { }
bool BWebWindow::AuthenticationChallenge(BString message,
                    BString& inOutUser, BString& inOutPassword,
                    bool& inOutRememberCredentials,
                    uint32 failureCount, BWebView* view) { return false; }
