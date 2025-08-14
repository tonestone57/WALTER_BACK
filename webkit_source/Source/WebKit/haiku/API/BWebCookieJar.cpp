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

#include "BWebCookieJar.h"

#include "API/APIHTTPCookieStore.h"
#include <WebCore/Cookie.h>
#include <wtf/URL.h>
#include <wtf/text/String.h>
#include <Invoker.h>
#include <Message.h>

BWebCookieJar::BWebCookieJar(API::HTTPCookieStore& cookieStore)
    : m_cookieStore(cookieStore)
{
}

BWebCookieJar::~BWebCookieJar()
{
}

void BWebCookieJar::GetCookies(const BUrl& url, BInvoker* completionInvoker) const
{
    WTF::URL wtfUrl(url);
    m_cookieStore.cookiesForURL(wtfUrl, [completionInvoker](Vector<WebCore::Cookie> cookies) {
        if (!completionInvoker)
            return;

        BMessage message(B_REPLY);
        for (const auto& cookie : cookies) {
            message.AddString("cookies", cookie.toString().utf8().data());
        }
        completionInvoker->Invoke(&message);
        delete completionInvoker;
    });
}

void BWebCookieJar::SetCookie(const BString& cookie, const BUrl& url)
{
    // A proper implementation would parse the cookie string.
    // For now, this is a placeholder.
    WebCore::Cookie newCookie;
    newCookie.name = "name"; // placeholder
    newCookie.value = "value"; // placeholder
    newCookie.domain = url.Host();
    newCookie.path = "/";
    newCookie.session = true;

    Vector<WebCore::Cookie> cookies;
    cookies.append(newCookie);

    m_cookieStore.setCookies(WTFMove(cookies), []{});
}

void BWebCookieJar::DeleteCookie(const BUrl& url, const char* name)
{
    // A proper implementation would need to get the cookie first to delete it.
    // This is a placeholder.
}
