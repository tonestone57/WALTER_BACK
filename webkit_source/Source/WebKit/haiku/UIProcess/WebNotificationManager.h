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

#pragma once

#include "WebNotificationManager.h"
#include <Handler.h>
#include <Notification.h>
#include <wtf/HashMap.h>
#include <wtf/UUID.h>

namespace WebKit {

class WebNotificationManagerHaiku final : public WebNotificationManager::Provider, public BHandler {
public:
    WebNotificationManagerHaiku(WebNotificationManager&);
    virtual ~WebNotificationManagerHaiku();

private:
    // BHandler
    void MessageReceived(BMessage*) override;

    // WebNotificationManager::Provider
    void showNotification(WebPageProxy&, const String&, const String&, const String&, const String&, WebCore::NotificationData&&, CompletionHandler<void(WebCore::NotificationClient::Permission)>&&) override;
    void cancelNotification(const UUID& notificationID) override;
    void clearNotifications(const Vector<UUID>& notificationIDs) override;
    void didDestroyNotification(const UUID& notificationID) override;

    WebNotificationManager& m_manager;
    HashMap<WTF::UUID, BNotification> m_notifications;
};

} // namespace WebKit
