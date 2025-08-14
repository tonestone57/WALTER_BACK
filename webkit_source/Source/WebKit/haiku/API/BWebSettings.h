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
#ifndef _B_WEB_SETTINGS_H_
#define _B_WEB_SETTINGS_H_

#include <Referenceable.h>
#include <String.h>

class BFont;
class BWebPage;

namespace WebKit {
class WebPreferences;
}

class BWebSettings : public BReferenceable {
public:
    static	void				SetIconDatabasePath(const BString& path);
    static	void				SetOfflineWebApplicationCachePath(const BString& path);

                                BWebSettings(BWebView& view);
    virtual						~BWebSettings();

    void						SetSerifFont(const BFont& font);
    void						SetSansSerifFont(const BFont& font);
    void						SetFixedFont(const BFont& font);
    void						SetStandardFont(const BFont& font);

    void						SetDefaultStandardFontSize(float size);
    void						SetDefaultFixedFontSize(float size);

    void						SetJavascriptEnabled(bool enable);
    void                        SetOfflineWebApplicationCacheEnabled(bool enable);

    void                        SetProxyInfo(const BString& host = "",
                                    uint32 port = 0,
                                    int32 type = 0, // BProxyType
                                    const BString& username = "",
                                    const BString& password = "");

    void                        SetDownloadPath(const BString& path);
    BString                     DownloadPath() const;

    void                        SetLocalStoragePath(const BString& path);
    BString                     LocalStoragePath() const;

private:
    B_DISABLE_COPY(BWebSettings);

    BWebPage&                   m_page;
    WebKit::WebPreferences&     m_preferences;
};

#endif // _B_WEB_SETTINGS_H_
