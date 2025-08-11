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

#include <Message.h>
#include <View.h>

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
