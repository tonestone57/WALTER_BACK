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
#include "WebDateTimePickerHaiku.h"

#include "WebPageProxy.h"
#include <Button.h>
#include <DateTime.h>
#include <DatePicker.h>
#include <LayoutBuilder.h>
#include <Looper.h>
#include <TimeSpinner.h>
#include <Window.h>

namespace WebKit {

constexpr uint32 kMsgOk = 'ok';
constexpr uint32 kMsgCancel = 'cncl';

class DateTimePickerWindow : public BWindow {
public:
    DateTimePickerWindow(WebDateTimePickerHaiku* dateTimePicker)
        : BWindow(BRect(100, 100, 400, 400), "Date/Time Picker", B_TITLED_WINDOW, 0)
        , m_dateTimePicker(dateTimePicker)
    {
    }

    virtual void Quit() override
    {
        m_dateTimePicker->endPicker();
        BWindow::Quit();
    }

private:
    WebDateTimePickerHaiku* m_dateTimePicker;
};


WebDateTimePickerHaiku::WebDateTimePickerHaiku(WebPageProxy& page, WebDateTimePicker::Client& client, const WebCore::IntRect& rect)
    : WebDateTimePicker(page, client, rect)
{
    be_app_looper->AddHandler(this);
}

WebDateTimePickerHaiku::~WebDateTimePickerHaiku()
{
    be_app_looper->RemoveHandler(this);
}

void WebDateTimePickerHaiku::showDateTimePicker()
{
    m_window = new DateTimePickerWindow(this);

    BLayoutBuilder::Group<> builder(B_VERTICAL, B_USE_DEFAULT_SPACING);
    builder.SetInsets(B_USE_WINDOW_INSETS);

    auto type = m_client.parameters().type;

    if (type == WebCore::InputType::Date || type == WebCore::InputType::DatetimeLocal) {
        m_datePicker = new BDatePicker("date_picker");
        builder.Add(m_datePicker);
    }
    if (type == WebCore::InputType::Time || type == WebCore::InputType::DatetimeLocal) {
        m_timeSpinner = new BTimeSpinner("time_spinner");
        builder.Add(m_timeSpinner);
    }

    BButton* okButton = new BButton("ok", "OK", new BMessage(kMsgOk));
    BButton* cancelButton = new BButton("cancel", "Cancel", new BMessage(kMsgCancel));

    builder.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
        .AddGlue()
        .Add(cancelButton)
        .Add(okButton)
    .End();

    m_window->AddChild(builder.View());

    okButton->SetTarget(this);
    cancelButton->SetTarget(this);

    // Set initial value from m_client.initialValue()
    const String& initialValue = m_client.initialValue();
    if (!initialValue.isEmpty()) {
        const char* valueStr = initialValue.utf8().data();
        if (m_datePicker) {
            int year, month, day;
            if (sscanf(valueStr, "%d-%d-%d", &year, &month, &day) == 3) {
                BDate date(year, month, day);
                if (date.IsValid())
                    m_datePicker->SetDate(date);
            }
        }
        if (m_timeSpinner) {
            const char* timeStr = strchr(valueStr, 'T');
            if (timeStr)
                timeStr++;
            else
                timeStr = valueStr;

            int hour, minute;
            if (sscanf(timeStr, "%d:%d", &hour, &minute) == 2) {
                BTime time(hour, minute, 0);
                if (time.IsValid())
                    m_timeSpinner->SetTime(time);
            }
        }
    }

    m_window->Show();
}

void WebDateTimePickerHaiku::endPicker()
{
    if (m_window) {
        m_window->Lock();
        m_window->Quit();
        m_window = nullptr;
    }
    m_client.didEndChooser();
}

void WebDateTimePickerHaiku::MessageReceived(BMessage* message)
{
    switch (message->what) {
    case kMsgOk: {
        String value;
        if (m_datePicker && m_timeSpinner) {
            BDateTime dt(m_datePicker->Date(), m_timeSpinner->Time());
            value = dt.ToString(B_FULL_DATE_FORMAT " " B_MEDIUM_TIME_FORMAT);
        } else if (m_datePicker) {
            value = BDate(m_datePicker->Date()).ToString();
        } else if (m_timeSpinner) {
            value = BTime(m_timeSpinner->Time()).ToString();
        }
        m_client.didChooseValue(value);
        endPicker();
        break;
    }
    case kMsgCancel:
        endPicker();
        break;
    default:
        BHandler::MessageReceived(message);
    }
}

} // namespace WebKit
