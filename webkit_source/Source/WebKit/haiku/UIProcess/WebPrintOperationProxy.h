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

#pragma once

#include "APIObject.h"
#include "PrintInfo.h"
#include <wtf/Ref.h>
#include <wtf/RefCounted.h>

namespace WebKit {

class WebPageProxy;

#include <private/print/PrintJob.h>

class WebPrintOperationProxy : public API::ObjectImpl<API::Object::Type::PrintOperation> {
public:
    static Ref<WebPrintOperationProxy> create(WebPageProxy&, const PrintInfo&);
    virtual ~WebPrintOperationProxy();

    void start();

    void didStartPrinting(int pageCount);
    void didRenderPage(const IPC::DataReference& pdfData, int pageNumber);
    void didFailPrinting();

private:
    WebPrintOperationProxy(WebPageProxy&, const PrintInfo&);

    void renderNextPage();
    void end();

    WebPageProxy& m_page;
    PrintInfo m_printInfo;
    RefPtr<BPrintJob> m_printJob;

    int m_pageCount { 0 };
    int m_currentPage { 0 };
};

} // namespace WebKit
