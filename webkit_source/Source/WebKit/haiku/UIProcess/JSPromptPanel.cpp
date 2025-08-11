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
#include "JSPromptPanel.h"

#include <Button.h>
#include <LayoutBuilder.h>
#include <StringView.h>
#include <TextView.h>

namespace WebKit {

JSPromptPanel::JSPromptPanel(const char* title, const char* message, const char* defaultValue,
    CompletionHandler<void(const String&)>&& completionHandler)
    : BWindow(BRect(100, 100, 400, 250), title, B_MODAL_WINDOW, B_ASYNCHRONOUS_CONTROLS)
    , m_completionHandler(WTFMove(completionHandler))
{
    m_textView = new BTextView("text_view");
    m_textView->SetText(defaultValue);

    m_okButton = new BButton("ok", "OK", new BMessage('ok'));
    m_cancelButton = new BButton("cancel", "Cancel", new BMessage('cncl'));

    BLayoutBuilder::Group<>(this, B_VERTICAL, B_USE_DEFAULT_SPACING)
        .SetInsets(B_USE_WINDOW_INSETS)
        .Add(new BStringView("message", message))
        .Add(m_textView)
        .AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
            .AddGlue()
            .Add(m_cancelButton)
            .Add(m_okButton)
        .End()
    .End();
}

void JSPromptPanel::MessageReceived(BMessage* message)
{
    switch (message->what) {
    case 'ok':
        m_completionHandler(String(m_textView->Text()));
        Quit();
        break;
    case 'cncl':
        m_completionHandler(String());
        Quit();
        break;
    default:
        BWindow::MessageReceived(message);
    }
}

} // namespace WebKit
