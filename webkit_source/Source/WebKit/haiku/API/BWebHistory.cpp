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

#include "BWebHistory.h"

#include "WebBackForwardList.h"
#include "WebBackForwardListItem.h"

BWebHistory::BWebHistory(WebKit::WebBackForwardList& list)
    : fList(list)
{
}

BWebHistory::~BWebHistory()
{
}

int32_t BWebHistory::CountItems() const
{
    return fList.entries().size();
}

BReference<BWebHistoryItem> BWebHistory::ItemAt(int32_t index) const
{
    if (index < 0 || (unsigned)index >= fList.entries().size())
        return nullptr;

    int32_t offset = index - fList.backListCount();
    WebKit::WebBackForwardListItem* item = fList.itemAtIndex(offset);
    if (!item)
        return nullptr;
    return new BWebHistoryItem(*item);
}

BReference<BWebHistoryItem> BWebHistory::ItemAtOffset(int32_t offset) const
{
    WebKit::WebBackForwardListItem* item = fList.itemAtIndex(offset);
    if (!item)
        return nullptr;
    return new BWebHistoryItem(*item);
}

BReference<BWebHistoryItem> BWebHistory::CurrentItem() const
{
    WebKit::WebBackForwardListItem* item = fList.currentItem();
    if (!item)
        return nullptr;
    return new BWebHistoryItem(*item);
}

int32_t BWebHistory::BackListCount() const
{
    return fList.backListCount();
}

int32_t BWebHistory::ForwardListCount() const
{
    return fList.forwardListCount();
}


BWebHistoryItem::BWebHistoryItem(WebKit::WebBackForwardListItem& item)
    : fItem(item)
    , fUrl(item.url().utf8().data())
    , fTitle(item.title().utf8().data())
{
}

BWebHistoryItem::~BWebHistoryItem()
{
}

const BString& BWebHistoryItem::URL() const
{
    return fUrl;
}

const BString& BWebHistoryItem::Title() const
{
    return fTitle;
}
