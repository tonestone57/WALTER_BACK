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
#include "WebPreferencesHaiku.h"

#include <WebCore/NotImplemented.h>

BWebSettings::BWebSettings()
    : fData(new BPrivate::WebSettingsPrivate())
{
	// This constructor is used only for the default (global) settings.
	if (be_app->Lock()) {
		be_app->AddHandler(this);
		be_app->Unlock();
	}
}

BWebSettings::BWebSettings(WebCore::Settings* settings)
    : fData(new BPrivate::WebSettingsPrivate(settings))
{
	if (be_app->Lock()) {
		be_app->AddHandler(this);
		be_app->Unlock();
	}
}

BWebSettings::~BWebSettings()
{
	if (be_app->Lock()) {
		be_app->RemoveHandler(this);
		be_app->Unlock();
	}
	delete fData;
}

BWebSettings* BWebSettings::Default()
{
	static BWebSettings defaultInstance;
	return &defaultInstance;
}

void BWebSettings::SetIconDatabasePath(const BString& path)
{
	notImplemented();
}

void BWebSettings::ClearIconDatabase()
{
	notImplemented();
}

void BWebSettings::SendIconForURL(const BString& url, const BMessage& reply,
		const BMessenger& target)
{
	notImplemented();
}


void BWebSettings::SetPersistentStoragePath(const BString& path)
{
	notImplemented();
}

void BWebSettings::SetOfflineStoragePath(const BString& path)
{
	notImplemented();
}

void BWebSettings::SetOfflineStorageDefaultQuota(int64 maximumSize)
{
	notImplemented();
}

void BWebSettings::SetOfflineWebApplicationCachePath(const BString& path)
{
	notImplemented();
}

void BWebSettings::SetOfflineWebApplicationCacheQuota(int64 maximumSize)
{
	notImplemented();
}

void BWebSettings::SetLocalStoragePath(const BString& path)
{
	notImplemented();
}

void BWebSettings::SetSerifFont(const BFont& font)
{
    notImplemented();
}

void BWebSettings::SetSansSerifFont(const BFont& font)
{
    notImplemented();
}

void BWebSettings::SetFixedFont(const BFont& font)
{
    notImplemented();
}

void BWebSettings::SetStandardFont(const BFont& font)
{
    notImplemented();
}

void BWebSettings::SetDefaultStandardFontSize(float size)
{
    notImplemented();
}

void BWebSettings::SetDefaultFixedFontSize(float size)
{
    notImplemented();
}

void BWebSettings::SetJavascriptEnabled(bool enabled)
{
	notImplemented();
}

void BWebSettings::SetProxyInfo(const BString& host, uint32 port,
	BProxyType type, const BString& username, const BString& password)
{
	notImplemented();
}

void BWebSettings::Apply()
{
    notImplemented();
}

// #pragma mark - private

void BWebSettings::_PostSetPath(BHandler* handler, uint32 what, const BString& path)
{
	notImplemented();
}

void BWebSettings::_PostSetQuota(BHandler* handler, uint32 what, int64 maximumSize)
{
	notImplemented();
}

void BWebSettings::_PostFont(uint32 which, const BFont& font)
{
	notImplemented();
}

void BWebSettings::_PostFontSize(uint32 which, float size)
{
	notImplemented();
}

void BWebSettings::_PostMessage(BHandler* handler, BMessage* message)
{
	notImplemented();
}

// #pragma mark -

void BWebSettings::MessageReceived(BMessage* message)
{
	notImplemented();
}

void BWebSettings::_HandleSetPersistentStoragePath(const BString& path)
{
    notImplemented();
}

void BWebSettings::_HandleSetOfflineStoragePath(const BString& path)
{
    notImplemented();
}

void BWebSettings::_HandleSetOfflineStorageDefaultQuota(int64 maximumSize)
{
    notImplemented();
}

void BWebSettings::_HandleSetWebApplicationCachePath(const BString& path)
{
    notImplemented();
}

void BWebSettings::_HandleSetIconDatabasePath(const BString& path)
{
	notImplemented();
}

void BWebSettings::_HandleClearIconDatabase()
{
	notImplemented();
}

void BWebSettings::_HandleSendIconForURL(BMessage* message)
{
	notImplemented();
}


void BWebSettings::_HandleSetWebApplicationCacheQuota(int64 maximumSize)
{
    notImplemented();
}

void BWebSettings::_HandleSetLocalStoragePath(const BString& path)
{
    notImplemented();
}

void BWebSettings::_HandleSetFont(BMessage* message)
{
	notImplemented();
}

void BWebSettings::_HandleSetFontSize(BMessage* message)
{
	notImplemented();
}


void BWebSettings::_HandleSetJavascriptEnabled(bool enable)
{
	notImplemented();
}


void BWebSettings::_HandleSetProxyInfo(BMessage* message)
{
	notImplemented();
}

void BWebSettings::_HandleApply()
{
	notImplemented();
}
