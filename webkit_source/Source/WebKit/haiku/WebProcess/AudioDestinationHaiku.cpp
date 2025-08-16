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
#include "AudioDestinationHaiku.h"

#if ENABLE(WEB_AUDIO)

#include <WebCore/AudioIOCallback.h>
#include <WebCore/NotImplemented.h>

namespace WebKit {

AudioDestinationHaiku::AudioDestinationHaiku(WebCore::AudioIOCallback& callback,
    unsigned numberOfInputChannels, unsigned numberOfOutputChannels, float sampleRate)
    : m_callback(callback)
{
    // TODO: Initialize BSoundPlayer here.
}

AudioDestinationHaiku::~AudioDestinationHaiku()
{
    // TODO: Clean up BSoundPlayer.
}

void AudioDestinationHaiku::start()
{
    // TODO: Start the BSoundPlayer.
    notImplemented();
}

void AudioDestinationHaiku::stop()
{
    // TODO: Stop the BSoundPlayer.
    notImplemented();
}

bool AudioDestinationHaiku::isPlaying() const
{
    // TODO: Return the state of the BSoundPlayer.
    notImplemented();
    return false;
}

float AudioDestinationHaiku::sampleRate() const
{
    // TODO: Return the actual sample rate.
    notImplemented();
    return 0;
}

} // namespace WebKit

#endif // ENABLE(WEB_AUDIO)
