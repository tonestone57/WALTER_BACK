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
#ifndef _B_WEB_HISTORY_H_
#define _B_WEB_HISTORY_H_

#include <Referenceable.h>
#include <String.h>

class BWebHistoryItem;
class BWebPage;

namespace WebKit {
class WebBackForwardList;
}

class BWebHistory : public BReferenceable {
public:
                                BWebHistory(WebKit::WebBackForwardList& list);
    virtual						~BWebHistory();
private:
    B_DISABLE_COPY(BWebHistory);

    int32_t						CountItems() const;
    BReference<BWebHistoryItem>	ItemAt(int32_t index) const;

    BReference<BWebHistoryItem>	CurrentItem() const;

private:
    WebKit::WebBackForwardList&	fList;
};

class BWebHistoryItem : public BReferenceable {
public:
                                BWebHistoryItem(WebKit::WebBackForwardListItem& item);
    virtual						~BWebHistoryItem();
private:
    B_DISABLE_COPY(BWebHistoryItem);

    const BString&				URL() const;
    const BString&				Title() const;

private:
    WebKit::WebBackForwardListItem& fItem;
    BString                     fUrl;
    BString                     fTitle;
};

#endif // _B_WEB_HISTORY_H_
