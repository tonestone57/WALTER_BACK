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

#include "BWebSettings.h"

#include "BWebView.h"
#include "BWebPage.h"
#include "WebPreferences.h"
#include <Font.h>

BWebSettings::BWebSettings(BWebView& view)
    : m_page(page)
    , m_preferences(view.page().preferences())
{
}

BWebSettings::~BWebSettings()
{
}

void BWebSettings::SetSerifFont(const BFont& font)
{
    font_family family;
    font_style style;
    font.GetFamilyAndStyle(&family, &style);
    m_preferences.setSerifFontFamily(family);
}

void BWebSettings::SetSansSerifFont(const BFont& font)
{
    font_family family;
    font_style style;
    font.GetFamilyAndStyle(&family, &style);
    m_preferences.setSansSerifFontFamily(family);
}

void BWebSettings::SetFixedFont(const BFont& font)
{
    font_family family;
    font_style style;
    font.GetFamilyAndStyle(&family, &style);
    m_preferences.setFixedFontFamily(family);
}

void BWebSettings::SetStandardFont(const BFont& font)
{
    font_family family;
    font_style style;
    font.GetFamilyAndStyle(&family, &style);
    m_preferences.setStandardFontFamily(family);
}

void BWebSettings::SetDefaultStandardFontSize(float size)
{
    m_preferences.setDefaultFontSize(size);
}

void BWebSettings::SetDefaultFixedFontSize(float size)
{
    m_preferences.setDefaultFixedFontSize(size);
}

void BWebSettings::SetJavascriptEnabled(bool enable)
{
    m_preferences.setJavaScriptEnabled(enable);
}

void BWebSettings::SetProxyInfo(const BString& host, uint32 port,
    int32 type, const BString& username, const BString& password)
{
    BUrlContext* context = new BUrlContext();
    context->SetProxy(host, port);
    // TODO: set type, username, password
    BUrlProtocolRoster::SetDefaultContext(context);

    m_page.page().process().processPool().networkProcess().send(
        Messages::NetworkProcess::SetNetworkProxySettings(), 0);
}
