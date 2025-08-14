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
 * THIS SOFTWARE IS PROVIDED BY HAIKU, INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL HAIKU, INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "StorageNamespaceProviderHaiku.h"

#include <WebCore/Page.h>
#include <WebCore/SecurityOrigin.h>
#include <WebCore/StorageArea.h>
#include <WebCore/StorageNamespace.h>
#include <WebCore/StorageTracker.h>
#include <WebCore/NotImplemented.h>

#include <Directory.h>
#include <FindDirectory.h>
#include <Path.h>

namespace WebKit {

Ref<StorageNamespaceProviderHaiku> StorageNamespaceProviderHaiku::create()
{
    return adoptRef(*new StorageNamespaceProviderHaiku);
}

StorageNamespaceProviderHaiku::StorageNamespaceProviderHaiku()
{
}

StorageNamespaceProviderHaiku::~StorageNamespaceProviderHaiku()
{
}

static WTF::String storagePath()
{
    static WTF::String path = [] {
        BPath settingsPath;
        if (find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath) != B_OK)
            return WTF::String();

        settingsPath.Append("WebKit/LocalStorage");
        create_directory(settingsPath.Path(), 0755);
        return settingsPath.Path();
    }();
    return path;
}

Ref<WebCore::StorageNamespace> StorageNamespaceProviderHaiku::createLocalStorageNamespace(unsigned quota, PAL::SessionID sessionID)
{
    if (!sessionID.isEphemeral())
        return WebCore::StorageNamespace::create(storagePath(), quota);

    // For ephemeral sessions, treat it like transient storage.
    return WebCore::StorageNamespace::create(nullptr, quota);
}

Ref<WebCore::StorageNamespace> StorageNamespaceProviderHaiku::createTransientLocalStorageNamespace(WebCore::SecurityOrigin&, unsigned quota, PAL::SessionID)
{
    // Transient storage is in-memory only.
    return WebCore::StorageNamespace::create(nullptr, quota);
}

RefPtr<WebCore::StorageNamespace> StorageNamespaceProviderHaiku::sessionStorageNamespace(const WebCore::SecurityOrigin&, WebCore::Page& page, ShouldCreateNamespace shouldCreate)
{
    if (shouldCreate == ShouldCreateNamespace::No) {
        if (m_sessionStorageNamespace)
            return m_sessionStorageNamespace;
        return nullptr;
    }

    if (m_sessionStorageNamespace)
        return m_sessionStorageNamespace;

    // Session storage is in-memory and has no quota.
    m_sessionStorageNamespace = WebCore::StorageNamespace::create(nullptr, WebCore::StorageArea::noQuota);
    return m_sessionStorageNamespace;
}

} // namespace WebKit
