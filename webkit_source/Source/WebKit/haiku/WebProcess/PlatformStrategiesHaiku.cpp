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
#include "PlatformStrategiesHaiku.h"

#include <WebCore/BlobRegistryImpl.h>
#include <WebCore/MediaStrategy.h>
#include <WebCore/NotImplemented.h>
#include <WebCore/ResourceLoadScheduler.h>

using namespace WebCore;

void PlatformStrategiesHaiku::initialize()
{
    static NeverDestroyed<PlatformStrategiesHaiku> platformStrategies;
    setPlatformStrategies(&platformStrategies.get());
}

PlatformStrategiesHaiku::PlatformStrategiesHaiku()
{
}

LoaderStrategy* PlatformStrategiesHaiku::createLoaderStrategy()
{
    return new ResourceLoadScheduler();
}

PasteboardStrategy* PlatformStrategiesHaiku::createPasteboardStrategy()
{
    notImplemented();
    return nullptr;
}

class WebBlobRegistry final : public BlobRegistry {
private:
    void registerInternalFileBlobURL(const URL& url, Ref<BlobDataFileReference>&& reference, const String& contentType, const String&) final override
    {
        m_blobRegistry.registerInternalFileBlobURL(url, WTFMove(reference), contentType);
    }
    void registerInternalBlobURL(const URL& url, Vector<BlobPart>&& parts, const String& contentType) final override
    {
        m_blobRegistry.registerInternalBlobURL(url, WTFMove(parts), contentType);
    }
    void registerBlobURL(const URL& url, const URL& srcURL, const PolicyContainer& container, const std::optional<SecurityOriginData>& topOrigin) final override
    {
        m_blobRegistry.registerBlobURL(url, srcURL, container, topOrigin);
    }
    void registerInternalBlobURLOptionallyFileBacked(const URL& url, const URL& srcURL, RefPtr<BlobDataFileReference>&& reference, const String& contentType) final override
    {
        m_blobRegistry.registerInternalBlobURLOptionallyFileBacked(url, srcURL, WTFMove(reference), contentType, { });
    }
    void registerInternalBlobURLForSlice(const URL& url, const URL& srcURL, long long start, long long end, const String& contentType) final override
    {
        m_blobRegistry.registerInternalBlobURLForSlice(url, srcURL, start, end, contentType);
    }
    void unregisterBlobURL(const URL& url, const std::optional<SecurityOriginData>& topOrigin) final override
    {
        m_blobRegistry.unregisterBlobURL(url, topOrigin);
    }
    void registerBlobURLHandle(const URL& url, const std::optional<SecurityOriginData>& topOrigin) final override
    {
        m_blobRegistry.registerBlobURLHandle(url, topOrigin);
    }
    void unregisterBlobURLHandle(const URL& url, const std::optional<SecurityOriginData>& topOrigin) final override
    {
        m_blobRegistry.unregisterBlobURLHandle(url, topOrigin);
    }
    unsigned long long blobSize(const URL& url) final override
    {
        return m_blobRegistry.blobSize(url);
    }
    String blobType(const URL& url) final override
    {
        return m_blobRegistry.blobType(url);
    }
    void writeBlobsToTemporaryFilesForIndexedDB(const Vector<String>& blobURLs, CompletionHandler<void(Vector<String>&& filePaths)>&& completionHandler) final override
    {
        m_blobRegistry.writeBlobsToTemporaryFilesForIndexedDB(blobURLs, WTFMove(completionHandler));
    }

    BlobRegistryImpl* blobRegistryImpl() final { return &m_blobRegistry; }

    BlobRegistryImpl m_blobRegistry;
};

WebCore::BlobRegistry* PlatformStrategiesHaiku::createBlobRegistry()
{
    return new WebBlobRegistry;
}

class WebMediaStrategy final : public MediaStrategy {
private:
#if ENABLE(WEB_AUDIO)
    std::unique_ptr<AudioDestination> createAudioDestination(AudioIOCallback& callback, const String& inputDeviceId,
        unsigned numberOfInputChannels, unsigned numberOfOutputChannels, float sampleRate) override
    {
        // FIXME: We should probably use the WebKit provided AudioDestination a la AudioDestinationCocoa
        notImplemented();
        return nullptr;
    }
#endif
};

MediaStrategy* PlatformStrategiesHaiku::createMediaStrategy()
{
    return new WebMediaStrategy;
}
