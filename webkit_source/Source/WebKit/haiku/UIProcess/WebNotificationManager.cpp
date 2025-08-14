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
#include "WebNotificationManager.h"

#include "WebPageProxy.h"
#include <Application.h>
#include <Bitmap.h>
#include <IconUtils.h>
#include <MimeType.h>
#include <Notification.h>
#include <WebCore/NotImplemented.h>
#include <WebCore/ResourceError.h>
#include <WebCore/Settings.h>

namespace WebKit {

std::unique_ptr<WebNotificationManager::Provider> WebNotificationManager::Provider::create(WebNotificationManager& manager)
{
    return makeUnique<WebNotificationManagerHaiku>(manager);
}

WebNotificationManagerHaiku::WebNotificationManagerHaiku(WebNotificationManager& manager)
    : m_manager(manager)
{
    be_app->LockLooper();
    be_app->AddHandler(this);
    be_app->UnlockLooper();
}

WebNotificationManagerHaiku::~WebNotificationManagerHaiku()
{
    be_app->LockLooper();
    be_app->RemoveHandler(this);
    be_app->UnlockLooper();
}

void WebNotificationManagerHaiku::showNotification(WebPageProxy& page, const String& title, const String& body, const String& iconURL, const String& tag, WebCore::NotificationData&& data, CompletionHandler<void(WebCore::NotificationClient::Permission)>&& completionHandler)
{
    auto permission = m_manager.permissionLevel(page.websiteDataStore(), data.origin);
    if (permission == WebCore::NotificationClient::Permission::Denied) {
        completionHandler(permission);
        return;
    }

    // TODO: Ask user for permission if it's NotAllowed.

    BNotification notification(B_INFORMATION_NOTIFICATION);
    notification.setGroup("WebKit");
    notification.setTitle(title.utf8().data());
    notification.setContent(body.utf8().data());
    notification.setNotificationID(tag.utf8().data());

    // TODO: Handle iconURL by fetching and decoding the image.
    // For now, use a generic icon.
    BBitmap icon(BRect(0, 0, 31, 31), B_RGBA32);
    if (BIconUtils::GetVectorIcon("application/x-vnd.Haiku-WebPositive", &icon) != B_OK)
        BIconUtils::GetVectorIcon("application/octet-stream", &icon);
    notification.setIcon(&icon);

    notification.SetOnClickApp(be_app_messenger);

    BMessage msg(B_SOME_APP_ACTIVATED);
    msg.AddString("notificationID", data.notificationID.toString().utf8().data());
    notification.SetOnClickMessage(&msg);

    notification.Send();

    m_notifications.add(data.notificationID, notification);
    m_manager.didShowNotification(data.notificationID);
    completionHandler(WebCore::NotificationClient::Permission::Granted);
}

void WebNotificationManagerHaiku::cancelNotification(const UUID& notificationID)
{
    if (m_notifications.contains(notificationID))
        m_notifications.remove(notificationID);
}

void WebNotificationManagerHaiku::clearNotifications(const Vector<UUID>& notificationIDs)
{
    for (const auto& id : notificationIDs) {
        if (m_notifications.contains(id))
            m_notifications.remove(id);
    }
}

void WebNotificationManagerHaiku::didDestroyNotification(const UUID& notificationID)
{
    if (m_notifications.contains(notificationID))
        m_notifications.remove(notificationID);
}

void WebNotificationManagerHaiku::MessageReceived(BMessage* message)
{
    if (message->what == B_SOME_APP_ACTIVATED) {
        const char* idStr;
        if (message->FindString("notificationID", &idStr) == B_OK) {
            if (auto notificationID = UUID::fromString(String::fromUTF8(idStr)))
                m_manager.didClickNotification(*notificationID);
        }
    } else {
        BHandler::MessageReceived(message);
    }
}

} // namespace WebKit
