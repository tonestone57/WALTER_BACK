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
#include "WebPrintOperationHaiku.h"

#include "WebErrors.h"
#include <WebCore/LocalFrame.h>
#include <WebCore/PrintContext.h>
#include <WebCore/ResourceError.h>
#include <wtf/TZoneMallocInlines.h>

#include <private/print/PrintJob.h>

namespace WebKit {

WTF_MAKE_TZONE_ALLOCATED_IMPL(WebPrintOperationHaiku);

WebPrintOperationHaiku::WebPrintOperationHaiku(const PrintInfo& printInfo)
    : m_printInfo(printInfo)
{
}

WebPrintOperationHaiku::~WebPrintOperationHaiku()
{
}

#include <WebCore/Document.h>
#include <WebCore/FrameView.h>

void WebPrintOperationHaiku::startPrint(WebCore::LocalFrame* frame, CompletionHandler<void(RefPtr<WebCore::FragmentedSharedBuffer>&&, WebCore::ResourceError&&)>&& completionHandler)
{
    m_completionHandler = WTFMove(completionHandler);

    String jobName = frame->document()->title();
    if (jobName.isEmpty())
        jobName = "WebKit Print output";

    m_printJob = new BPrintJob(jobName.utf8().data());
    if (m_printInfo.printSettings)
        m_printJob->SetSettings(new BMessage(*m_printInfo.printSettings));

    m_printContext = std::make_unique<WebCore::PrintContext>(*frame);
    m_printContext->begin(m_printJob->PrintableRect().Width(), m_printJob->PrintableRect().Height());

    m_pageCount = m_printContext->pageCount();
    bool success = true;

    if (m_pageCount > 0) {
        if (m_printJob->BeginJob() != B_OK) {
            success = false;
        } else {
            float printedPageWidth = m_printJob->PrintableRect().Width();

            // FIXME: This should be done in an idle task to avoid blocking the main thread.
            for (int i = 1; i <= m_pageCount; ++i) {
                if (m_printJob->BeginPage(i) != B_OK) {
                    success = false;
                    break;
                }
                WebCore::GraphicsContext graphicsContext(m_printJob->View());
                m_printContext->spoolPage(graphicsContext, i, printedPageWidth);
                if (m_printJob->CommitPage() != B_OK) {
                    success = false;
                    break;
                }
            }

            if (success && m_printJob->CommitJob() != B_OK) {
                success = false;
            }
        }
    }

    endPrint(success);
}

void WebPrintOperationHaiku::endPrint(bool success)
{
    if (!m_completionHandler)
        return;

    if (success) {
        m_completionHandler(nullptr, { });
    } else {
        // FIXME: Create a better ResourceError
        m_completionHandler(nullptr, WebCore::ResourceError(String(), -1, m_printContext->frame().document()->url(), "Printing failed"_s, WebCore::ResourceError::Type::General));
    }
}

} // namespace WebKit
