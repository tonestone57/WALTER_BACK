/*
 * Copyright 2017, Adrien Destugues, pulkomandy@pulkomandy.tk
 * Distributed under terms of the MIT license.
 */


#include "NotificationClientHaiku.h"
#include "IconDownloader.h"
#include "WebCore/NotificationData.h"

#include "WebPage.h"

namespace WebCore {

BNotification
NotificationClientHaiku::fromDescriptor(NotificationData& descriptor)
{
    BNotification notification(B_INFORMATION_NOTIFICATION);
    notification.SetGroup("WebPositive");
    // Unfortunately, we don't get a website name or so…
    if (descriptor.body.length() > 0) {
        notification.SetTitle(descriptor.title);
        notification.SetContent(descriptor.body);
    } else {
        notification.SetContent(descriptor.title);
    }

#if !USE(CURL)
    // TODO we should cache the data, in case the notification is re-sent
    // with some changes for an update.
    if (!descriptor.iconURL.isEmpty()) {
        IconDownloader* downloader = new IconDownloader(notification);
        downloader->Start(BUrl(descriptor.iconURL.string()));
    }
#endif

    notification.SetMessageID(descriptor.tag);

    return notification;
}

};
