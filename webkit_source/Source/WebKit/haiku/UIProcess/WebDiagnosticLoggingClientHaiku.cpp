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
#include "WebDiagnosticLoggingClientHaiku.h"

#include <WebCore/NotImplemented.h>
#include <WebCore/Settings.h>
#include <stdio.h>

namespace WebKit {

using namespace WebCore;

void WebDiagnosticLoggingClientHaiku::logDiagnosticMessage(const String& message, const String& description, WebCore::ShouldSample shouldSample)
{
    if (!shouldLogAfterSampling(shouldSample))
        return;
    fprintf(stderr, "%s: %s\n", message.utf8().data(), description.utf8().data());
}

void WebDiagnosticLoggingClientHaiku::logDiagnosticMessageWithResult(const String& message, const String& description, WebCore::DiagnosticLoggingResultType, WebCore::ShouldSample shouldSample)
{
    if (!shouldLogAfterSampling(shouldSample))
        return;
    fprintf(stderr, "%s: %s\n", message.utf8().data(), description.utf8().data());
}

void WebDiagnosticLoggingClientHaiku::logDiagnosticMessageWithValue(const String& message, const String& description, double value, unsigned, WebCore::ShouldSample shouldSample)
{
    if (!shouldLogAfterSampling(shouldSample))
        return;
    fprintf(stderr, "%s: %s (%f)\n", message.utf8().data(), description.utf8().data(), value);
}

void WebDiagnosticLoggingClientHaiku::logDiagnosticMessageWithEnhancedPrivacy(const String& message, const String& description, WebCore::ShouldSample shouldSample)
{
    if (!shouldLogAfterSampling(shouldSample))
        return;
    fprintf(stderr, "%s: %s\n", message.utf8().data(), description.utf8().data());
}

void WebDiagnosticLoggingClientHaiku::logDiagnosticMessageWithValueDictionary(const String& message, const String& description, const WebCore::DiagnosticLoggingClient::ValueDictionary&, WebCore::ShouldSample shouldSample)
{
    if (!shouldLogAfterSampling(shouldSample))
        return;
    fprintf(stderr, "%s: %s\n", message.utf8().data(), description.utf8().data());
}

void WebDiagnosticLoggingClientHaiku::logDiagnosticMessageWithDomain(const String& message, WebCore::DiagnosticLoggingDomain)
{
    fprintf(stderr, "%s\n", message.utf8().data());
}

} // namespace WebKit
