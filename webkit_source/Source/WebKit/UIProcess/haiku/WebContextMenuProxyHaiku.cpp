/*
 * Copyright (C) 2021 Haiku, Inc.
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

#if ENABLE(CONTEXT_MENUS)

#include "APIContextMenuClient.h"
#include "NativeWebMouseEvent.h"
#include "WebContextMenuItem.h"
#include "WebPageProxy.h"
#include "WebView.h"
#include <wtf/text/CString.h>

#include <ContextMenu.h>
#include <MenuItem.h>
#include <Menu.h>

namespace WebKit {
using namespace WebCore;

WebContextMenuProxyHaiku::WebContextMenuProxyHaiku(BWebView* webView, WebPageProxy& page, FrameInfoData&& frameInfo, ContextMenuContextData&& context, const UserData& userData)
    : WebContextMenuProxy(page, WTFMove(context), userData)
    , m_webView(webView)
    , m_frameInfo(WTFMove(frameInfo))
{
}

WebContextMenuProxyHaiku::~WebContextMenuProxyHaiku()
{
}

Vector<Ref<WebContextMenuItem>> WebContextMenuProxyHaiku::proposedItems() const
{
    Vector<Ref<WebContextMenuItem>> proposedAPIItems;
    for (auto& item : m_context.menuItems()) {
        proposedAPIItems.append(WebContextMenuItem::create(item));
    }
    return proposedAPIItems;
}

void WebContextMenuProxyHaiku::show()
{
    if (m_context.type() != ContextMenuContext::Type::ContextMenu) {
        useContextMenuItems(proposedItems());
        return;
    }

    WebContextMenuProxy::show();
}

void WebContextMenuProxyHaiku::showContextMenuWithItems(Vector<Ref<WebContextMenuItem>>&& items)
{
    BMenu* menu = new BMenu("context menu");
    for (const auto& item : items) {
        if (item->data().type() == ContextMenuItemType::Separator) {
            menu->AddSeparatorItem();
        } else if (item->data().type() == ContextMenuItemType::Submenu) {
            BMenu* subMenu = new BMenu(item->data().title().utf8().data());
            // FIXME: The WebContextMenuItemData doesn't contain the submenu items.
            // We need to add a submenu() method to WebContextMenuItemData that returns a WebContextMenu object.
            // This requires changes in files that are not part of the Haiku port.
            menu->AddItem(subMenu);
        } else {
            BMessage* message = new BMessage(B_EXECUTE);
            message->AddInt32("action", item->data().action());
            BMenuItem* menuItem = new BMenuItem(item->data().title().utf8().data(), message);
            menuItem->SetEnabled(item->data().enabled());
            menuItem->SetTarget(m_webView);
            menu->AddItem(menuItem);
        }
    }

    BPoint point(m_context.menuLocation().x(), m_context.menuLocation().y());
    m_webView->ConvertToScreen(&point);
    menu->Go(point, true, true, true);
    delete menu;
}

} // namespace WebKit

#endif // ENABLE(CONTEXT_MENUS)
