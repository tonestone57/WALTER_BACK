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
#include "NativeWebContextMenuHaiku.h"

#include "WebContextMenuItemData.h"
#include <MenuItem.h>

namespace WebKit {

NativeWebContextMenuHaiku::NativeWebContextMenuHaiku(const Vector<WebContextMenuItemData>& items)
    : BPopUpMenu("context menu", false, false)
    , m_items(items)
{
    for (const auto& item : m_items) {
        if (item.type() == WebCore::SeparatorType) {
            AddSeparatorItem();
            continue;
        }

        BMessage* message = new BMessage(item.action());
        BMenuItem* menuItem = new BMenuItem(item.title().utf8().data(), message);
        AddItem(menuItem);
    }
}

const WebContextMenuItemData* NativeWebContextMenuHaiku::Go(BPoint where, bool delivers_message, bool open_anyway, bool async)
{
    BMenuItem* selected = BPopUpMenu::Go(where, delivers_message, open_anyway, async);
    if (!selected)
        return nullptr;

    const BMessage* message = selected->Message();
    if (!message)
        return nullptr;

    int32 action = message->what;
    for (const auto& item : m_items) {
        if (item.action() == action)
            return &item;
    }

    return nullptr;
}

} // namespace WebKit
