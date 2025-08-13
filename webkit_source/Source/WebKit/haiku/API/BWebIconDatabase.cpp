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
 *    documentation and/or other materials provided with a a distribution.
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

#include "BWebIconDatabase.h"

#include "WebContext.h"
#include "WebIconDatabase.h"
#include <WebCore/BitmapImage.h>
#include <WebCore/NativeImage.h>
#include <WebCore/ShareableBitmap.h>
#include <Bitmap.h>
#include <Directory.h>
#include <FindDirectory.h>
#include <Path.h>
#include <Message.h>

BWebIconDatabase* BWebIconDatabase::sDefault = nullptr;

BWebIconDatabase* BWebIconDatabase::Default()
{
    if (!sDefault)
        sDefault = new BWebIconDatabase();
    return sDefault;
}

BWebIconDatabase::BWebIconDatabase()
    : m_database(WebKit::WebContext::singleton().iconDatabase())
{
    BPath path;
    if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
        path.Append("webkit/icondatabase.db");
        SetPath(path.Path());
    }
}

BWebIconDatabase::~BWebIconDatabase()
{
    sDefault = nullptr;
}

void BWebIconDatabase::SetPath(const BString& path)
{
    m_database.setDatabasePath(path.String());
}

BString BWebIconDatabase::Path() const
{
    return m_database.databasePath().utf8().data();
}

BBitmap* BWebIconDatabase::IconForURL(const BString& url, bool checkWhileLoading)
{
    RefPtr<WebCore::Image> image = m_database.iconForPageURL(url.String(), WebCore::IntSize(16, 16), checkWhileLoading);
    if (!image)
        return nullptr;

    if (auto nativeImage = image->nativeImage())
        return new BBitmap(nativeImage->platformImage().get());

    return nullptr;
}

void BWebIconDatabase::SetIconForURL(const BString& url, const BBitmap* icon)
{
    if (!icon)
        return;

    auto nativeImage = WebCore::NativeImage::create(icon);
    auto bitmapImage = WebCore::BitmapImage::create(WTFMove(nativeImage));

    if (auto sharedBitmap = WebCore::ShareableBitmap::create(bitmapImage->size(), { })) {
        auto graphicsContext = sharedBitmap->createGraphicsContext();
        if (graphicsContext)
            graphicsContext->drawImage(*bitmapImage, WebCore::FloatRect({ }, bitmapImage->size()));

        m_database.setIconDataForIconURL(sharedBitmap->createHandle(), url.String());
    }
}

void BWebIconDatabase::Clear()
{
    m_database.removeAllIcons();
}
