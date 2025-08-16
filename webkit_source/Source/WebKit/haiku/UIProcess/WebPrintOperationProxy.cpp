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
#include "WebPrintOperationProxy.h"

#include "DrawingAreaProxy.h"
#include "MessageSenderInlines.h"
#include "WebPageProxy.h"
#include "WebPageProxyMessages.h"
#include "WebPrintOperationProxyMessages.h"
#include "WebProcessProxy.h"
#include <Alert.h>
#include <WebCore/PrintInfo.h>
#include <wtf/text/WTFString.h>

namespace WebKit {

using namespace WebCore;

Ref<WebPrintOperationProxy> WebPrintOperationProxy::create(WebPageProxy& page, const PrintInfo& printInfo)
{
    return adoptRef(*new WebPrintOperationProxy(page, printInfo));
}

WebPrintOperationProxy::WebPrintOperationProxy(WebPageProxy& page, const PrintInfo& printInfo)
    : m_page(page)
    , m_printInfo(printInfo)
    , m_printJob(new BPrintJob(m_page.pageName().utf8().data()))
{
    m_page.process().addMessageReceiver(Messages::WebPrintOperationProxy::messageReceiverName(), m_page.pageID(), *this);
}

void WebPrintOperationProxy::start()
{
    if (m_printJob->ConfigJob() != B_OK) {
        end();
        return;
    }

    m_printJob->BeginJob();

    // FIXME: Use the actual frame. The public API would need to be changed to
    // allow specifying a frame to print.
    m_page.send(Messages::WebPageProxy::PrintFrame(m_page.mainFrameID(), m_printInfo));
}

void WebPrintOperationProxy::didStartPrinting(int pageCount)
{
    m_pageCount = pageCount;
    m_currentPage = 0;
    renderNextPage();
}

void WebPrintOperationProxy::didRenderPage(const IPC::DataReference& pdfData, int pageNumber)
{
    // FIXME: The data is in PDF format, but the Haiku printing system expects
    // a BPicture for each page. To fix this, we would need to either:
    //   a) Link a PDF rendering library (like Poppler) into the UI process
    //      to convert the PDF data into a sequence of drawing commands that
    //      can be stored in a BPicture.
    //   b) Change the WebProcess to render to a BPicture directly instead of
    //      a PDF context. This would require a new GraphicsContext backend.
    //
    // For now, printing is non-functional. We just advance the page count
    // to avoid getting stuck.
    // m_printJob->SpoolPage();

    m_currentPage++;
    if (m_currentPage < m_pageCount)
        renderNextPage();
    else
        end();
}

void WebPrintOperationProxy::didFailPrinting()
{
    auto* alert = new BAlert("Printing Failed", "The print job could not be completed.", "OK");
    alert->Go(nullptr);
    end();
}

void WebPrintOperationProxy::renderNextPage()
{
    m_page.send(Messages::WebPageProxy::RenderPageForPrinting(m_page.mainFrameID(), m_currentPage));
}

void WebPrintOperationProxy::end()
{
    m_printJob->CommitJob();
    m_page.process().removeMessageReceiver(Messages::WebPrintOperationProxy::messageReceiverName(), m_page.pageID());
    m_page.didFinishPrintOperation(this);
}

} // namespace WebKit
