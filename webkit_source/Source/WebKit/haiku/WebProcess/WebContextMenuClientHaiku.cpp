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
#include "WebContextMenuClientHaiku.h"

#include "WebPage.h"
#include "WebPageHaiku.h"
#include "WebProcess.h"
#include "WebPageProxyMessages.h"
#include <WebCore/ContextMenu.h>
#include <WebCore/Editor.h>
#include <WebCore/FrameLoader.h>
#include <WebCore/NotImplemented.h>
#include <WebCore/Page.h>
#include <stdio.h>
#include <WebCore/ResourceRequest.h>
#include <WebCore/TextIterator.h>
#include <WebCore/UserGestureIndicator.h>

namespace WebKit {

using namespace WebCore;

WebContextMenuClientHaiku::WebContextMenuClientHaiku(WebPage* page)
    : m_page(page)
{
}

void WebContextMenuClientHaiku::downloadURL(const URL& url)
{
    static_cast<WebPageHaiku*>(m_page)->downloadManager().startDownload(ResourceRequest(url));
}

void WebContextMenuClientHaiku::searchWithGoogle(const LocalFrame* frame)
{
    String searchString = plainText(frame->selection().selection());
    String encoded = WTF::encodeWithURLEscapeSequences(searchString.trim(deprecatedIsSpaceOrNewline));
    encoded = makeStringByReplacingAll(encoded, "%20"_s, "+"_s);

    StringBuilder url;
    url.append("http://www.google.com/search?q="_s);
    url.append(encoded);

    if (Page* page = frame->page()) {
        UserGestureIndicator indicator(IsProcessingUserGesture::Yes);
        page->process().send(Messages::WebPageProxy::CreateNewPage({ }, ResourceRequest(URL(url))), page->identifier());
    }
}

void WebContextMenuClientHaiku::lookUpInDictionary(LocalFrame*)
{
    fprintf(stderr, "WebContextMenuClientHaiku::lookUpInDictionary: Unimplemented\n");
}

void WebContextMenuClientHaiku::speak(const String&)
{
    fprintf(stderr, "WebContextMenuClientHaiku::speak: Unimplemented\n");
}

bool WebContextMenuClientHaiku::isSpeaking() const
{
    fprintf(stderr, "WebContextMenuClientHaiku::isSpeaking: Unimplemented\n");
    return false;
}

void WebContextMenuClientHaiku::stopSpeaking()
{
    fprintf(stderr, "WebContextMenuClientHaiku::stopSpeaking: Unimplemented\n");
}

} // namespace WebKit
