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
#include "WebPopupMenuProxyHaiku.h"

#include "WebPageProxy.h"
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <SeparatorItem.h>
#include <View.h>

namespace WebKit {

WebPopupMenuProxyHaiku::WebPopupMenuProxyHaiku(WebPageProxy& page, WebPopupMenuProxy::Client& client)
    : WebPopupMenuProxy(page, client)
{
}

void WebPopupMenuProxyHaiku::showPopupMenu(const WebCore::IntRect& rect, WebCore::TextDirection, double, const Vector<WebPopupItem>& items, const PlatformMethod&, int32_t)
{
    BPopUpMenu menu("popup", true, true);

    for (const auto& item : items) {
        if (item.m_type == WebPopupItem::Separator) {
            menu.AddSeparatorItem();
        } else {
            BMenuItem* menuItem = new BMenuItem(item.m_text.utf8().data(), nullptr);
            menuItem->SetEnabled(item.m_isEnabled);
            menuItem->SetMarked(item.m_isSelected);
            menu.AddItem(menuItem);
        }
    }

    BView* view = m_page.view();
    if (!view) {
        m_client->didCancel();
        return;
    }

    BPoint screenLocation(rect.x(), rect.y() + rect.height());
    view->ConvertToScreen(&screenLocation);

    BMenuItem* selectedItem = menu.Go(screenLocation, false, true);

    if (selectedItem) {
        int32_t selectedIndex = menu.IndexOf(selectedItem);
        m_client->didChooseItem(selectedIndex);
    } else {
        m_client->didCancel();
    }
}

void WebPopupMenuProxyHaiku::hidePopupMenu()
{
    // The BPopUpMenu::Go() call is synchronous, so the menu is already
    // hidden by the time this function could be called in the same thread.
    // If WebKit calls this from another thread, we would need a more complex
    // asynchronous implementation with message passing to handle it safely.
    // For now, we assume the synchronous behavior is sufficient.
}

} // namespace WebKit
