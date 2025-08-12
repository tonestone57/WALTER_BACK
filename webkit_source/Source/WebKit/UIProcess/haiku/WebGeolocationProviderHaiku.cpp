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

#include "WebGeolocationProviderHaiku.h"

#include "WebGeolocationManagerProxy.h"
#include "WebGeolocationPosition.h"

namespace WebKit {

WebGeolocationProviderHaiku::WebGeolocationProviderHaiku(WebGeolocationManagerProxy& manager)
    : m_manager(manager)
    , m_running(false)
    , m_enableHighAccuracy(false)
{
}

WebGeolocationProviderHaiku::~WebGeolocationProviderHaiku()
{
    stopUpdating(&m_manager);
}

void WebGeolocationProviderHaiku::startUpdating(WebGeolocationManagerProxy*)
{
    if (m_running)
        return;

    m_running = true;
    m_thread = std::thread(&WebGeolocationProviderHaiku::locationThread, this);
}

void WebGeolocationProviderHaiku::stopUpdating(WebGeolocationManagerProxy*)
{
    if (!m_running)
        return;

    m_running = false;
    if (m_thread.joinable())
        m_thread.join();
}

void WebGeolocationProviderHaiku::setEnableHighAccuracy(WebGeolocationManagerProxy*, bool enable)
{
    m_enableHighAccuracy = enable;
}

void WebGeolocationProviderHaiku::locationThread()
{
    while (m_running) {
        float latitude, longitude;
        if (m_geolocation.LocateSelf(latitude, longitude) == B_OK) {
            auto position = WebGeolocationPosition::create(std::chrono::system_clock::now().time_since_epoch().count(), latitude, longitude, 0);
            m_manager.providerDidChangePosition(position.get());
        } else {
            m_manager.providerDidFailToDeterminePosition();
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

} // namespace WebKit
