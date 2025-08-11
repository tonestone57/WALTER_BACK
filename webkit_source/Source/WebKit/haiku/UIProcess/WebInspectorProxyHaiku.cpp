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
#include "WebInspectorProxyHaiku.h"

#include "WebPageProxy.h"
#include <WebCore/NotImplemented.h>

namespace WebKit {

WebInspectorProxyHaiku::WebInspectorProxyHaiku(WebPageProxy& page)
    : WebInspectorProxy(page)
{
}

WebInspectorProxyHaiku::~WebInspectorProxyHaiku()
{
}

Ref<WebPageProxy> WebInspectorProxyHaiku::createInspectorPage(Ref<API::PageConfiguration>&& configuration)
{
    notImplemented();
    return WebInspectorProxy::createInspectorPage(WTFMove(configuration));
}

String WebInspectorProxyHaiku::inspectorURL() const
{
    return "resource:///org/webkit/inspector/UserInterface/Main.html"_s;
}

String WebInspectorProxyHaiku::inspectorPageURL() const
{
    return "resource:///org/webkit/inspector/UserInterface/Main.html"_s;
}

String WebInspectorProxyHaiku::inspectorTestPageURL() const
{
    return "resource:///org/webkit/inspector/UserInterface/Test.html"_s;
}

#include "WebPageProxyHaiku.h"
#include "WebView.h"
#include <Window.h>

void WebInspectorProxyHaiku::platformCreateInspectorWindow()
{
    if (m_inspectorWindow) {
        m_inspectorWindow->Activate();
        return;
    }

    m_inspectorWindow = new BWindow(BRect(100, 100, 900, 700), "Web Inspector", B_TITLED_WINDOW, 0);

    Ref<API::PageConfiguration> configuration = API::PageConfiguration::create();
    Ref<WebPageProxy> inspectorPage = createInspectorPage(WTFMove(configuration));
    m_inspectorView = static_cast<WebPageProxyHaiku*>(inspectorPage.ptr())->view();

    m_inspectorWindow->AddChild(m_inspectorView);
    m_inspectorWindow->Show();
}

void WebInspectorProxyHaiku::platformCloseInspectorWindow()
{
    if (m_inspectorWindow) {
        m_inspectorWindow->Lock();
        m_inspectorWindow->Quit();
        m_inspectorWindow = nullptr;
        m_inspectorView = nullptr;
    }
}

void WebInspectorProxyHaiku::platformBringToFront()
{
    if (m_inspectorWindow)
        m_inspectorWindow->Activate();
}

void WebInspectorProxyHaiku::platformDidClose()
{
    m_inspectorWindow = nullptr;
    m_inspectorView = nullptr;
}

bool WebInspectorProxyHaiku::platformIsFront()
{
    if (m_inspectorWindow)
        return m_inspectorWindow->IsActive();
    return false;
}

void WebInspectorProxyHaiku::platformAttach()
{
    notImplemented();
}

void WebInspectorProxyHaiku::platformDetach()
{
    notImplemented();
}

} // namespace WebKit
