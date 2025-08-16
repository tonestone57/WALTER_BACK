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
#include "WebContextMenuProxyHaiku.h"

#include "WebContextMenuItemData.h"
#include "WebPageProxyHaiku.h"
#include <WebCore/ContextMenuContextData.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <Window.h>

namespace WebKit {

WebContextMenuProxyHaiku::WebContextMenuProxyHaiku(WebPageProxy& page, ContextMenuContextData&& context, const UserData& userData)
    : WebContextMenuProxy(page, WTFMove(context), userData)
{
}

void WebContextMenuProxyHaiku::show()
{
    Vector<WebContextMenuItemData> menuItems = m_context.menuItems();

    if (!m_context.linkURL().isEmpty()) {
        menuItems.insert(0, WebContextMenuItemData(WebCore::ActionType,
            static_cast<WebCore::ContextMenuAction>(WebCore::ContextMenuAction::LastAPIAction + 2),
            "Download Linked File"_s, true, false));
        menuItems.insert(0, WebContextMenuItemData(WebCore::ActionType,
            static_cast<WebCore::ContextMenuAction>(WebCore::ContextMenuAction::LastAPIAction + 1),
            "Open Link in New Window"_s, true, false));
        menuItems.insert(2, WebContextMenuItemData(WebCore::SeparatorType,
            static_cast<WebCore::ContextMenuAction>(0), String(), false, false));
    }

    BPopUpMenu* menu = new BPopUpMenu("context menu", false, false);
    for (const auto& item : menuItems) {
        if (item.type() == WebCore::SeparatorType) {
            menu->AddSeparatorItem();
            continue;
        }
        BMessage* message = new BMessage(item.action());
        BMenuItem* menuItem = new BMenuItem(item.title().utf8().data(), message);
        menu->AddItem(menuItem);
        menuItem->SetEnabled(item.enabled());
    }

    WebPageProxyHaiku& page = static_cast<WebPageProxyHaiku&>(m_page.get());
    BPoint where;
    uint32 buttons;
    page.view()->GetMouse(&where, &buttons, true);
    BPoint screen_where = page.view()->ConvertToScreen(where);

    BMenuItem* selected = menu->Go(screen_where, false, true, true);
    if (selected) {
        if (const BMessage* message = selected->Message()) {
            int32 action = message->what;
            for (const auto& item : menuItems) {
                if (item.action() == action) {
                    m_page->contextMenuItemSelected(item);
                    break;
                }
            }
        }
    }

    // Notify the page that the menu has been dismissed. This allows the page
    // to release this WebContextMenuProxy object.
    m_page->didDismissContextMenu();
}

void WebContextMenuProxyHaiku::cancel()
{
    // BPopUpMenu::Go is synchronous and handles its own dismissal,
    // so there is nothing to do here. The menu will be dismissed when
    // show() returns, and didDismissContextMenu() will be called there.
}

} // namespace WebKit
