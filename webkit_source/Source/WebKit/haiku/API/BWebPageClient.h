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

#ifndef B_WEB_PAGE_CLIENT_H
#define B_WEB_PAGE_CLIENT_H

#include <String.h>

class BWebView;

class BWebPageClient {
public:
    virtual ~BWebPageClient() = default;

    virtual void TitleChanged(const BString& title, BWebView* view) { }
    virtual void LoadCommitted(const BString& url, BWebView* view) { }
    virtual void LoadFinished(const BString& url, BWebView* view) { }
    virtual void LoadProgress(float progress, BWebView* view) { }
    virtual void NavigationCapabilitiesChanged(bool canGoBackward,
        bool canGoForward, bool canStop, BWebView* view) { }
    virtual BWebView* CreateInspectorWindow() { return nullptr; }
    virtual bool AuthenticationChallenge(const BString& message, BString& inOutUser,
        BString& inOutPassword, bool& inOutRememberCredentials) { return false; }
    virtual BWebView* CreateNewWindow() { return nullptr; }
};

#endif // B_WEB_PAGE_CLIENT_H
