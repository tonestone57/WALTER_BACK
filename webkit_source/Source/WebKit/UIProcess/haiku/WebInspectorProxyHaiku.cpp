/*
 * Copyright (C) 2014 Haiku, inc.
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
#include "WebInspectorProxy.h"

#if ENABLE(INSPECTOR)

#include "WebProcessProxy.h"
#include <WebCore/NotImplemented.h>
#include <WebKit/WKPage.h>
#include <WebKit/WKPageGroup.h>
#include <WebKit/WKString.h>
#include <wtf/text/CString.h>
#include <wtf/text/StringBuilder.h>
#include <wtf/text/WTFString.h>

namespace WebKit {

void WebInspectorProxy::platformHide()
{
}

void WebInspectorProxy::platformResetState()
{
}

void WebInspectorProxy::platformBringToFront()
{
}

bool WebInspectorProxy::platformIsFront()
{
    return false;
}

void WebInspectorProxy::platformInspectedURLChanged(const String& url)
{
}

String WebInspectorProxy::inspectorPageURL()
{
    StringBuilder builder;
    builder.append(inspectorBaseURL());
    builder.appendLiteral("/Main.html");

    return builder.toString();
}

String WebInspectorProxy::inspectorTestPageURL()
{
    StringBuilder builder;
    builder.append(inspectorBaseURL());
    builder.appendLiteral("/Test.html");

    return builder.toString();
}

String WebInspectorProxy::inspectorBaseURL()
{
    // FIXME: This needs to be implemented to return the path to the inspector resources.
    // This is likely a file:// URL pointing to the Resources/inspector directory
    // in the application bundle.
    return "file://";
}

unsigned WebInspectorProxy::platformInspectedWindowHeight()
{
    return 0;
}

unsigned WebInspectorProxy::platformInspectedWindowWidth()
{
    return 0;
}

void WebInspectorProxy::platformAttach()
{
}

void WebInspectorProxy::platformDetach()
{
}

void WebInspectorProxy::platformSetAttachedWindowHeight(unsigned)
{
}

void WebInspectorProxy::platformSetAttachedWindowWidth(unsigned)
{
}

void WebInspectorProxy::platformSave(const String&, const String&, bool, bool)
{
}

void WebInspectorProxy::platformAppend(const String&, const String&)
{
}

void WebInspectorProxy::platformAttachAvailabilityChanged(bool)
{
}

void WebInspectorProxy::platformStartWindowDrag()
{
}

void WebInspectorProxy::platformCreateFrontendWindow()
{
    // FIXME: This needs to be implemented to create a new window for the inspector.
    // This will involve creating a BWindow and a WebView, and loading the inspector frontend.
}

void WebInspectorProxy::platformCloseFrontendPageAndWindow()
{
}

void WebInspectorProxy::platformShowCertificate(const WebCore::CertificateInfo&)
{
}

void WebInspectorProxy::platformDidCloseForCrash()
{
}

void WebInspectorProxy::platformInvalidate()
{
}

void WebInspectorProxy::platformBringInspectedPageToFront()
{
}

WebPageProxy* WebInspectorProxy::platformCreateFrontendPage()
{
    // FIXME: This needs to be implemented to create a new WebPageProxy for the inspector.
    // This will likely involve creating a new WebView with a special configuration.
    return nullptr;
}

}

#endif
