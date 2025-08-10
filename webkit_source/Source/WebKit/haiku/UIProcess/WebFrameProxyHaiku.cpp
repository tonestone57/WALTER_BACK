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
#include "WebFrameProxy.h"
#include "WebFrameProxyHaiku.h"

#include <WebCore/NotImplemented.h>

namespace WebKit {

void WebFrameProxy::platformInvalidate()
{
    // Haiku-specific invalidation of the WebFrameProxy.
    // This is where you would tear down any platform-specific resources associated with the frame proxy.
}

void BWebFrame::SetListener(const BMessenger& listener)
{
    notImplemented();
}

void BWebFrame::LoadURL(BString url)
{
    notImplemented();
}

void BWebFrame::StopLoading()
{
    notImplemented();
}

void BWebFrame::Reload()
{
    notImplemented();
}

BString BWebFrame::RequestedURL() const
{
    notImplemented();
    return BString();
}

BString BWebFrame::URL() const
{
    notImplemented();
    return BString();
}

BString BWebFrame::MIMEType() const
{
    notImplemented();
    return BString();
}

bool BWebFrame::CanCopy() const
{
    notImplemented();
    return false;
}

bool BWebFrame::CanCut() const
{
    notImplemented();
    return false;
}

bool BWebFrame::CanPaste() const
{
    notImplemented();
    return false;
}

void BWebFrame::Copy()
{
    notImplemented();
}

void BWebFrame::Cut()
{
    notImplemented();
}

void BWebFrame::Paste()
{
    notImplemented();
}

bool BWebFrame::CanUndo() const
{
    notImplemented();
    return false;
}

bool BWebFrame::CanRedo() const
{
    notImplemented();
    return false;
}

void BWebFrame::Undo()
{
    notImplemented();
}

void BWebFrame::Redo()
{
    notImplemented();
}

bool BWebFrame::AllowsScrolling() const
{
    notImplemented();
    return false;
}

void BWebFrame::SetAllowsScrolling(bool enable)
{
    notImplemented();
}

BPoint BWebFrame::ScrollPosition()
{
    notImplemented();
    return BPoint();
}

BString BWebFrame::FrameSource() const
{
    notImplemented();
    return BString();
}

void BWebFrame::SetFrameSource(const BString& source)
{
    notImplemented();
}

void BWebFrame::SetTransparent(bool transparent)
{
    notImplemented();
}

bool BWebFrame::IsTransparent() const
{
    notImplemented();
    return false;
}

BString BWebFrame::InnerText() const
{
    notImplemented();
    return BString();
}

BString BWebFrame::AsMarkup() const
{
    notImplemented();
    return BString();
}

BString BWebFrame::ExternalRepresentation() const
{
    notImplemented();
    return BString();
}

bool BWebFrame::FindString(const BString& string,
                                    WebCore::FindOptions options)
{
    notImplemented();
    return false;
}

bool BWebFrame::CanIncreaseZoomFactor() const
{
    notImplemented();
    return false;
}

bool BWebFrame::CanDecreaseZoomFactor() const
{
    notImplemented();
    return false;
}

void BWebFrame::IncreaseZoomFactor(bool textOnly)
{
    notImplemented();
}

void BWebFrame::DecreaseZoomFactor(bool textOnly)
{
    notImplemented();
}

void BWebFrame::ResetZoomFactor()
{
    notImplemented();
}

void BWebFrame::SetEditable(bool editable)
{
    notImplemented();
}

bool BWebFrame::IsEditable() const
{
    notImplemented();
    return false;
}

void BWebFrame::SetTitle(const BString& title)
{
    notImplemented();
}

const BString& BWebFrame::Title() const
{
    notImplemented();
    static BString title;
    return title;
}

const char* BWebFrame::Name() const
{
    notImplemented();
    return nullptr;
}

JSGlobalContextRef BWebFrame::GlobalContext() const
{
    notImplemented();
    return nullptr;
}


} // namespace WebKit
