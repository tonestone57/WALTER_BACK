/*
 * Copyright (C) 2022-2024 Apple Inc. All rights reserved.
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
#include "ArgumentCodersHaiku.h"

#include "PrintInfo.h"
#include "WebCoreArgumentCoders.h"
#include <Messenger.h>
#include <Message.h>
#include <wtf/Vector.h>

namespace IPC {

void ArgumentCoder<BMessenger>::encode(Encoder& encoder, const BMessenger& messenger)
{
    encoder << messenger.Team();
    encoder << messenger.Target();
}

void ArgumentCoder<BMessenger>::encode(Encoder& encoder, BMessenger&& messenger)
{
    ArgumentCoder<BMessenger>::encode(encoder, messenger);
}

std::optional<BMessenger> ArgumentCoder<BMessenger>::decode(Decoder& decoder)
{
    team_id team;
    if (!decoder.decode(team))
        return std::nullopt;

    port_id port;
    if (!decoder.decode(port))
        return std::nullopt;

    return BMessenger(team, port);
}

void ArgumentCoder<WebKit::PrintInfo>::encode(Encoder& encoder, const WebKit::PrintInfo& printInfo)
{
    encoder << printInfo.pageSetupScaleFactor;
    encoder << printInfo.availablePaperWidth;
    encoder << printInfo.availablePaperHeight;
    encoder << printInfo.margin;
    if (!printInfo.printSettings) {
        encoder << false;
        return;
    }
    encoder << true;

    ssize_t settingsSize = printInfo.printSettings->FlattenedSize();
    Vector<uint8_t> settingsBuffer(settingsSize);
    if (printInfo.printSettings->Flatten(reinterpret_cast<char*>(settingsBuffer.data()), settingsSize) == B_OK)
        encoder << settingsBuffer;
    else
        encoder << Vector<uint8_t>();
}

std::optional<WebKit::PrintInfo> ArgumentCoder<WebKit::PrintInfo>::decode(Decoder& decoder)
{
    WebKit::PrintInfo printInfo;
    if (!decoder.decode(printInfo.pageSetupScaleFactor))
        return std::nullopt;
    if (!decoder.decode(printInfo.availablePaperWidth))
        return std::nullopt;
    if (!decoder.decode(printInfo.availablePaperHeight))
        return std::nullopt;
    if (!decoder.decode(printInfo.margin))
        return std::nullopt;

    bool hasSettings;
    if (!decoder.decode(hasSettings))
        return std::nullopt;

    if (!hasSettings)
        return printInfo;

    std::optional<Vector<uint8_t>> settingsBuffer;
    decoder.decode(settingsBuffer);
    if (!settingsBuffer)
        return std::nullopt;

    if (settingsBuffer->isEmpty())
        return printInfo;

    printInfo.printSettings = std::make_unique<BMessage>();
    if (printInfo.printSettings->Unflatten(reinterpret_cast<const char*>(settingsBuffer->data())) != B_OK)
        printInfo.printSettings.reset();

    return printInfo;
}

} // namespace IPC
