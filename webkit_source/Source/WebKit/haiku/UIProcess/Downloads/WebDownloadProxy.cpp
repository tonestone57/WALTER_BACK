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
#include "WebDownloadProxy.h"

#include <WebCore/NotImplemented.h>

BWebDownload::BWebDownload(BPrivate::WebDownloadPrivate* data)
    : fData(data)
{
	notImplemented();
}

BWebDownload::~BWebDownload()
{
	notImplemented();
}

void BWebDownload::Start(const BPath& path)
{
	notImplemented();
}

void BWebDownload::HasMovedTo(const BPath& path)
{
	notImplemented();
}

void BWebDownload::Cancel()
{
	notImplemented();
}

void BWebDownload::SetProgressListener(const BMessenger& listener)
{
	notImplemented();
}

const BString& BWebDownload::URL() const
{
	notImplemented();
	static BString url;
	return url;
}

const BPath& BWebDownload::Path() const
{
	notImplemented();
	static BPath path;
	return path;
}

const BString& BWebDownload::Filename() const
{
	notImplemented();
	static BString filename;
	return filename;
}

off_t BWebDownload::CurrentSize() const
{
	notImplemented();
	return 0;
}

off_t BWebDownload::ExpectedSize() const
{
	notImplemented();
	return 0;
}

// #pragma mark - private

void BWebDownload::MessageReceived(BMessage* message)
{
	notImplemented();
}

void BWebDownload::_HandleCancel()
{
	notImplemented();
}
