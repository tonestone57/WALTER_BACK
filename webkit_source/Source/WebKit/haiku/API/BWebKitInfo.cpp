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

#include "BWebKitInfo.h"

#include <WebKitVersion.h>
#include <String.h>

/*static*/ BString
BWebKitInfo::HaikuWebKitVersion()
{
    return "2.0.0"; // This should be defined in a Haiku-specific version header
}

/*static*/ BString
BWebKitInfo::WebKitVersion()
{
    return BString() << WEBKIT_MAJOR_VERSION << "." << WEBKIT_MINOR_VERSION << "." << WEBKIT_TINY_VERSION;
}

/*static*/ int
BWebKitInfo::WebKitMajorVersion()
{
    return WEBKIT_MAJOR_VERSION;
}

/*static*/ int
BWebKitInfo::WebKitMinorVersion()
{
    return WEBKIT_MINOR_VERSION;
}

/*static*/ int
BWebKitInfo::WebKitTinyVersion()
{
    return WEBKIT_TINY_VERSION;
}
