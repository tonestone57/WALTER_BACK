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
#include "WebColorPickerHaiku.h"

#include "WebPageProxy.h"
#include <ColorControl.h>
#include <Looper.h>
#include <Window.h>

namespace WebKit {

class ColorPickerWindow : public BWindow {
public:
    ColorPickerWindow(WebColorPickerHaiku* colorPicker)
        : BWindow(BRect(100, 100, 400, 400), "Color Picker", B_TITLED_WINDOW, 0)
        , m_colorPicker(colorPicker)
    {
    }

    virtual void Quit() override
    {
        m_colorPicker->endPicker();
        BWindow::Quit();
    }

private:
    WebColorPickerHaiku* m_colorPicker;
};


WebColorPickerHaiku::WebColorPickerHaiku(WebPageProxy& page, const WebCore::Color& color, const WebCore::IntRect& rect)
    : WebColorPicker(page, color, rect)
{
    be_app_looper->AddHandler(this);
}

WebColorPickerHaiku::~WebColorPickerHaiku()
{
    be_app_looper->RemoveHandler(this);
}

void WebColorPickerHaiku::showColorPicker(const WebCore::Color& color)
{
    m_window = new ColorPickerWindow(this);
    BColorControl* colorControl = new BColorControl(BPoint(10, 10), B_CELLS_32x8, 8, "color_control", new BMessage('clch'));
    m_window->AddChild(colorControl);
    colorControl->SetTarget(this);
    colorControl->SetValue(color.rgb());
    m_window->Show();
}

void WebColorPickerHaiku::endPicker()
{
    if (m_window) {
        m_window->Lock();
        m_window->Quit();
        m_window = nullptr;
    }
    m_page->didEndColorPicker();
}

void WebColorPickerHaiku::MessageReceived(BMessage* message)
{
    switch (message->what) {
    case 'clch': {
        if (BColorControl* colorControl = dynamic_cast<BColorControl*>(message->FindView("source"))) {
            rgb_color c = colorControl->ValueAsColor();
            m_page->didChooseColor(WebCore::Color(c.red, c.green, c.blue, c.alpha));
        }
        break;
    }
    default:
        BHandler::MessageReceived(message);
    }
}

} // namespace WebKit
