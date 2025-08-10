/*
 * Copyright (C) 2024 Haiku, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the aove copyright
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
#include "WebTextCheckerClientHaiku.h"

#include <WebCore/TextChecking.h>

namespace WebKit {

bool WebTextCheckerClientHaiku::shouldEraseMarkersAfterChangeSelection(WebCore::TextCheckingType) const
{
    // TODO: Implement
    return true;
}

void WebTextCheckerClientHaiku::ignoreWordInSpellDocument(const String&)
{
    // TODO: Implement
}

void WebTextCheckerClientHaiku::learnWord(const String&)
{
    // TODO: Implement
}

void WebTextCheckerClientHaiku::checkSpellingOfWord(const String&, CompletionHandler<void(int32_t, int32_t)>&& completionHandler)
{
    // TODO: Implement
    completionHandler(-1, -1);
}

void WebTextCheckerClientHaiku::checkGrammarOfSentence(const String&, CompletionHandler<void(Vector<WebCore::GrammarDetail>&&)>&& completionHandler)
{
    // TODO: Implement
    completionHandler({ });
}

void WebTextCheckerClientHaiku::getGuessesForWord(const String&, const String&, int32_t, CompletionHandler<void(Vector<String>&&)>&& completionHandler)
{
    // TODO: Implement
    completionHandler({ });
}

void WebTextCheckerClientHaiku::requestCheckingOfString(uint64_t, const WebCore::TextCheckingRequestData&, int32_t, CompletionHandler<void(Vector<WebCore::TextCheckingResult>&&)>&& completionHandler)
{
    // TODO: Implement
    completionHandler({ });
}

void WebTextCheckerClientHaiku::cancelCheckingOfString(uint64_t)
{
    // TODO: Implement
}

} // namespace WebKit
