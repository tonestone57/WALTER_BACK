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
 * ARISING IN ANY WAY OUT of THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebContextMenuProxyHaiku.h"

#include "WebPageProxy.h"
#include "WebContextMenuItemData.h"
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <Window.h>

namespace WebKit {

WebContextMenuProxyHaiku::WebContextMenuProxyHaiku(WebPageProxy& page, ContextMenuContextData&& context, const UserData& userData)
    : WebContextMenuProxy(page, WTFMove(context), userData)
{
}

void WebContextMenuProxyHaiku::showContextMenu()
{
    BPopUpMenu* menu = new BPopUpMenu("context menu", false, false);
    for (const auto& item : m_context.menuItems()) {
        if (item.type() == WebCore::SeparatorType) {
            menu->AddSeparatorItem();
            continue;
        }
        BMessage* message = new BMessage(item.action());
        BMenuItem* menuItem = new BMenuItem(item.title().utf8().data(), message);
        menu->AddItem(menuItem);
    }

    BRect mouseRect;
    uint32 buttons;
    m_page.view()->GetMouse(&mouseRect.leftTop(), &buttons, true);
    mouseRect.rightTop() = mouseRect.leftTop();
    m_page.view()->ConvertToScreen(&mouseRect);

    BMenuItem* selected = menu->Go(mouseRect.leftTop(), false, true, true);
    if (selected) {
        if (const BMessage* message = selected->Message()) {
            int32 action = message->what;
            for (const auto& item : m_context.menuItems()) {
                if (item.action() == action) {
                    m_page.contextMenuItemSelected(item);
                    break;
                }
            }
        }
    }
}

void WebContextMenuProxyHaiku::cancelContextMenu()
{
    // TODO: Implement
}

} // namespace WebKit
