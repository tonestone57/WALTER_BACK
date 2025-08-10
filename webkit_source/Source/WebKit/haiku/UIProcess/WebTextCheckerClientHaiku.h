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

#pragma once

#include "WebTextCheckerClient.h"

namespace WebKit {

class WebTextCheckerClientHaiku final : public WebTextCheckerClient {
public:
    WebTextCheckerClientHaiku() = default;
    virtual ~WebTextCheckerClientHaiku() = default;

private:
    bool shouldEraseMarkersAfterChangeSelection(WebCore::TextCheckingType) const override;
    void ignoreWordInSpellDocument(const String&) override;
    void learnWord(const String&) override;
    void checkSpellingOfWord(const String&, CompletionHandler<void(int32_t, int32_t)>&&) override;
    void checkGrammarOfSentence(const String&, CompletionHandler<void(Vector<WebCore::GrammarDetail>&&)>&&) override;
    void getGuessesForWord(const String&, const String&, int32_t, CompletionHandler<void(Vector<String>&&)>&&) override;
    void requestCheckingOfString(uint64_t, const WebCore::TextCheckingRequestData&, int32_t, CompletionHandler<void(Vector<WebCore::TextCheckingResult>&&)>&&) override;
    void cancelCheckingOfString(uint64_t) override;
};

} // namespace WebKit
