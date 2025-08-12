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
#include "WebCookieManager.h"

#include <NetworkKit/NetworkCookieJar.h>
#include <WebCore/Cookie.h>
#include <WebCore/URL.h>

namespace WebKit {

void WebCookieManager::getHostnamesWithCookies(PAL::SessionID sessionID, CompletionHandler<void(Vector<String>&&)>&& completionHandler)
{
    Vector<String> hostnames;
    if (m_cookieJars.contains(sessionID)) {
        auto& cookieJar = m_cookieJars.get(sessionID);
        for (auto it = cookieJar.GetIterator(); it.HasNext();) {
            const BNetworkCookie* cookie = it.Next();
            hostnames.append(String(cookie->Domain()));
        }
    }
    completionHandler(WTFMove(hostnames));
}

void WebCookieManager::deleteCookie(PAL::SessionID sessionID, const WebCore::Cookie& cookie, CompletionHandler<void()>&& completionHandler)
{
    if (m_cookieJars.contains(sessionID)) {
        auto& cookieJar = m_cookieJars.get(sessionID);
        // BNetworkCookieJar has no simple way to delete a single cookie.
        // We have to iterate and find it.
        for (auto it = cookieJar.GetIterator(); it.HasNext();) {
            const BNetworkCookie* bcookie = it.Next();
            if (String(bcookie->Name()) == cookie.name && String(bcookie->Domain()) == cookie.domain && String(bcookie->Path()) == cookie.path) {
                it.Remove();
                break;
            }
        }
    }
    completionHandler();
}

void WebCookieManager::setCookies(PAL::SessionID sessionID, const Vector<WebCore::Cookie>& cookies, const URL&, const URL&, CompletionHandler<void()>&& completionHandler)
{
    if (!m_cookieJars.contains(sessionID))
        m_cookieJars.add(sessionID, BPrivate::Network::BNetworkCookieJar());

    auto& cookieJar = m_cookieJars.get(sessionID);
    for (const auto& cookie : cookies) {
        BNetworkCookie bcookie(cookie.name.utf8().data(), cookie.value.utf8().data());
        bcookie.SetDomain(cookie.domain.utf8().data());
        bcookie.SetPath(cookie.path.utf8().data());
        bcookie.SetExpiryTime(cookie.expires.value_or(0));
        bcookie.SetSecure(cookie.secure);
        bcookie.SetHttpOnly(cookie.httpOnly);
        cookieJar.AddCookie(bcookie);
    }
    completionHandler();
}

void WebCookieManager::deleteAllCookiesModifiedSince(PAL::SessionID sessionID, WallTime time, CompletionHandler<void()>&& completionHandler)
{
    if (m_cookieJars.contains(sessionID)) {
        auto& cookieJar = m_cookieJars.get(sessionID);
        for (auto it = cookieJar.GetIterator(); it.HasNext();) {
            const BNetworkCookie* bcookie = it.Next();
            if (bcookie->CreationTime() >= time.secondsSinceEpoch().count()) {
                it.Remove();
            }
        }
    }
    completionHandler();
}

void WebCookieManager::deleteAllCookies(PAL::SessionID sessionID, CompletionHandler<void()>&& completionHandler)
{
    if (m_cookieJars.contains(sessionID)) {
        m_cookieJars.remove(sessionID);
    }
    completionHandler();
}

void WebCookieManager::deleteCookiesForHostnames(PAL::SessionID sessionID, const Vector<String>& hostnames, CompletionHandler<void()>&& completionHandler)
{
    if (m_cookieJars.contains(sessionID)) {
        auto& cookieJar = m_cookieJars.get(sessionID);
        for (const auto& hostname : hostnames) {
            for (auto it = cookieJar.GetIterator(); it.HasNext();) {
                const BNetworkCookie* bcookie = it.Next();
                if (String(bcookie->Domain()) == hostname) {
                    it.Remove();
                }
            }
        }
    }
    completionHandler();
}

void WebCookieManager::getAllCookies(PAL::SessionID sessionID, CompletionHandler<void(Vector<WebCore::Cookie>&&)>&& completionHandler)
{
    Vector<WebCore::Cookie> cookies;
    if (m_cookieJars.contains(sessionID)) {
        auto& cookieJar = m_cookieJars.get(sessionID);
        for (auto it = cookieJar.GetIterator(); it.HasNext();) {
            const BNetworkCookie* bcookie = it.Next();
            WebCore::Cookie cookie;
            cookie.name = bcookie->Name();
            cookie.value = bcookie->Value();
            cookie.domain = bcookie->Domain();
            cookie.path = bcookie->Path();
            cookie.expires = bcookie->ExpiryTime();
            cookie.httpOnly = bcookie->IsHTTPOnly();
            cookie.secure = bcookie->IsSecure();
            cookie.session = false; // BNetworkCookie does not have this info
            cookies.append(WTFMove(cookie));
        }
    }
    completionHandler(WTFMove(cookies));
}

void WebCookieManager::getCookies(PAL::SessionID sessionID, const URL& url, CompletionHandler<void(Vector<WebCore::Cookie>&&)>&& completionHandler)
{
    Vector<WebCore::Cookie> cookies;
    if (m_cookieJars.contains(sessionID)) {
        auto& cookieJar = m_cookieJars.get(sessionID);
        BUrl burl(url);
        for (auto it = cookieJar.GetUrlIterator(burl); it.HasNext();) {
            const BNetworkCookie* bcookie = it.Next();
            WebCore::Cookie cookie;
            cookie.name = bcookie->Name();
            cookie.value = bcookie->Value();
            cookie.domain = bcookie->Domain();
            cookie.path = bcookie->Path();
            cookie.expires = bcookie->ExpiryTime();
            cookie.httpOnly = bcookie->IsHTTPOnly();
            cookie.secure = bcookie->IsSecure();
            cookie.session = false; // BNetworkCookie does not have this info
            cookies.append(WTFMove(cookie));
        }
    }
    completionHandler(WTFMove(cookies));
}


void WebCookieManager::platformSetHTTPCookieAcceptPolicy(PAL::SessionID, WebCore::HTTPCookieAcceptPolicy policy, CompletionHandler<void()>&& completionHandler)
{
    // TODO: Implement
    completionHandler();
}

} // namespace WebKit
