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

#include "BWebPage.h"
#include "BWebView.h"
#include "BWebWindow.h"

#include <Message.h>
#include <View.h>
#include <Window.h>

BWebPage::BWebPage(BWebView* view)
    : BWebPageClient()
    , fWebView(view)
    , m_dragSource(fWebView->page())
    , m_dragDestination(fWebView->page())
{
}

BWebPage::~BWebPage()
{
}


void BWebPage::MouseMoved(BPoint where, uint32 transit, const BMessage* dragMessage)
{
    if (dragMessage) {
        switch (transit) {
        case B_ENTERED_VIEW:
            m_dragDestination.DragEntered(dragMessage, where);
            break;
        case B_INSIDE_VIEW:
            m_dragDestination.DragUpdated(dragMessage, where);
            break;
        case B_EXITED_VIEW:
            m_dragDestination.DragExited(dragMessage, where);
            break;
        }
    }
}

void BWebPage::MessageReceived(BMessage* message)
{
    switch (message->what) {
    case B_SIMPLE_DATA:
        m_dragDestination.Drop(message, BPoint());
        break;
    }
}

bool BWebPage::CanGoBack()
{
    return fWebView->page().canGoBack();
}

bool BWebPage::CanGoForward()
{
    return fWebView->page().canGoForward();
}

void BWebPage::GoBack()
{
    fWebView->page().goBack();
}

void BWebPage::GoForward()
{
    fWebView->page().goForward();
}

void BWebPage::FindString(const char* string, bool forward,
    bool caseSensitive, bool wrapSelection, bool startInSelection)
{
    WebKit::FindOptions options;
    if (forward)
        options.add(WebKit::FindOptions::DoNotUpdateAfterFinding);
    if (!caseSensitive)
        options.add(WebKit::FindOptions::CaseInsensitive);
    if (wrapSelection)
        options.add(WebKit::FindOptions::WrapAround);
    if (startInSelection)
        options.add(WebKit::FindOptions::AtWordStarts);

    fWebView->page().findString(String::fromUTF8(string), options, 1000, [](bool success) {
        // TODO: report success back to the application
    });
}

void BWebPage::IncreaseZoomFactor(bool textOnly)
{
    if (textOnly)
        fWebView->page().setTextZoomFactor(fWebView->page().textZoomFactor() * 1.2);
    else
        fWebView->page().setPageZoomFactor(fWebView->page().pageZoomFactor() * 1.2);
}

void BWebPage::DecreaseZoomFactor(bool textOnly)
{
    if (textOnly)
        fWebView->page().setTextZoomFactor(fWebView->page().textZoomFactor() / 1.2);
    else
        fWebView->page().setPageZoomFactor(fWebView->page().pageZoomFactor() / 1.2);
}

#include "WebPreferences.h"

void BWebPage::ResetZoomFactor()
{
    fWebView->page().setTextZoomFactor(1);
    fWebView->page().setPageZoomFactor(1);
}

void BWebPage::SetDarkMode(bool dark)
{
    fWebView->page().preferences().setDarkModeEnabled(dark);
}

// BWebPageClient hooks
void BWebPage::NavigationRequested(const BString& url, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->NavigationRequested(url, view);
}

void BWebPage::NewWindowRequested(const BString& url, bool primaryAction)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->NewWindowRequested(url, primaryAction);
}

void BWebPage::CloseWindowRequested(BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->CloseWindowRequested(view);
}

void BWebPage::NewPageCreated(BWebView* view, BRect windowFrame, bool modalDialog, bool resizable, bool activate)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->NewPageCreated(view, windowFrame, modalDialog, resizable, activate);
}

void BWebPage::LoadNegotiating(const BString& url, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->LoadNegotiating(url, view);
}

void BWebPage::LoadCommitted(const BString& url, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->LoadCommitted(url, view);
}

void BWebPage::LoadFailed(const BString& url, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->LoadFailed(url, view);
}

void BWebPage::LoadFinished(const BString& url, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->LoadFinished(url, view);
}

void BWebPage::LoadProgress(float progress, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->LoadProgress(progress, view);
}

void BWebPage::MainDocumentError(const BString& failingURL, const BString& localizedDescription, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->MainDocumentError(failingURL, localizedDescription, view);
}

void BWebPage::TitleChanged(const BString& title, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->TitleChanged(title, view);
}

void BWebPage::IconReceived(const BBitmap* icon, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->IconReceived(icon, view);
}

void BWebPage::ResizeRequested(float width, float height, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->ResizeRequested(width, height, view);
}

void BWebPage::SetToolBarsVisible(bool flag, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->SetToolBarsVisible(flag, view);
}

void BWebPage::SetStatusBarVisible(bool flag, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->SetStatusBarVisible(flag, view);
}

void BWebPage::SetMenuBarVisible(bool flag, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->SetMenuBarVisible(flag, view);
}

void BWebPage::SetResizable(bool flag, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->SetResizable(flag, view);
}

void BWebPage::StatusChanged(const BString& status, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->StatusChanged(status, view);
}

void BWebPage::NavigationCapabilitiesChanged(bool canGoBackward,
    bool canGoForward, bool canStop, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->NavigationCapabilitiesChanged(canGoBackward, canGoForward, canStop, view);
}

void BWebPage::UndoRedoStateChanged(BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->UndoRedoStateChanged(view);
}

void BWebPage::UpdateGlobalHistory(const BString& url)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        window->UpdateGlobalHistory(url);
}

bool BWebPage::AuthenticationChallenge(BString message,
                    BString& inOutUser, BString& inOutPassword,
                    bool& inOutRememberCredentials,
                    uint32 failureCount, BWebView* view)
{
    if (BWebWindow* window = static_cast<BWebWindow*>(fWebView->Window()))
        return window->AuthenticationChallenge(message, inOutUser, inOutPassword, inOutRememberCredentials, failureCount, view);
    return false;
}
