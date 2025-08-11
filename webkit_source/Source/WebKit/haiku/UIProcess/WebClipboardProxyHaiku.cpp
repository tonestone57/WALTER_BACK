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
#include "WebClipboardProxyHaiku.h"

#include <WebCore/PasteboardCustomData.h>
#include <WebCore/SharedBuffer.h>
#include <Clipboard.h>
#include <Message.h>

namespace WebKit {

void WebClipboardProxyHaiku::getTypes(CompletionHandler<void(Vector<String>&&)>&& completionHandler)
{
    Vector<String> types;
    if (be_clipboard->Lock()) {
        BMessage* data = be_clipboard->Data();
        if (data) {
            char* name;
            uint32 type;
            int32 count;
            for (int32 i = 0; data->GetInfo(B_MIME_TYPE, i, &name, &type, &count) == B_OK; i++) {
                types.append(name);
            }
        }
        be_clipboard->Unlock();
    }
    completionHandler(WTFMove(types));
}

#include <Bitmap.h>

void WebClipboardProxyHaiku::write(const WebCore::PasteboardCustomData& customData, CompletionHandler<void(int64_t)>&& completionHandler)
{
    if (be_clipboard->Lock()) {
        be_clipboard->Clear();
        BMessage* data = be_clipboard->Data();
        if (data) {
            for (auto& item : customData.bundleData()) {
                if (item.first == "image/png") {
                    auto bitmap = ShareableBitmap::create(item.second);
                    if (bitmap) {
                        BBitmap* bbitmap = bitmap->haikuBitmap();
                        if (bbitmap) {
                            BMessage flattenedBitmap;
                            bbitmap->Archive(&flattenedBitmap);
                            data->AddMessage("image/png", &flattenedBitmap);
                        }
                    }
                }
            }
            be_clipboard->Commit();
        }
        be_clipboard->Unlock();
    }
    completionHandler(0);
}

void WebClipboardProxyHaiku::read(const String& type, CompletionHandler<void(WebCore::PasteboardCustomData&&)>&& completionHandler)
{
    if (type != "image/png") {
        completionHandler({ });
        return;
    }

    if (be_clipboard->Lock()) {
        BMessage* data = be_clipboard->Data();
        if (data) {
            BMessage flattenedBitmap;
            if (data->FindMessage("image/png", &flattenedBitmap) == B_OK) {
                BBitmap* bbitmap = new BBitmap(&flattenedBitmap);
                if (bbitmap) {
                    BBitmapStream stream(bbitmap);
                    off_t size;
                    stream.GetSize(&size);
                    auto buffer = WebCore::SharedBuffer::create(size);
                    stream.Read(buffer->data(), size);

                    WebCore::PasteboardCustomData customData;
                    customData.setData("image/png", WTFMove(buffer));
                    completionHandler(WTFMove(customData));
                    return;
                }
            }
        }
        be_clipboard->Unlock();
    }

    completionHandler({ });
}

void WebClipboardProxyHaiku::files(CompletionHandler<void(Vector<String>&&)>&& completionHandler)
{
    // TODO: Implement
    completionHandler({ });
}

void WebClipboardProxyHaiku::write(const WebCore::SharedBuffer& buffer, const String& type, CompletionHandler<void(int64_t)>&& completionHandler)
{
    if (type != "text/plain" && type != "text/html") {
        completionHandler(0);
        return;
    }

    if (be_clipboard->Lock()) {
        be_clipboard->Clear();
        BMessage* data = be_clipboard->Data();
        if (data) {
            data->AddData(type.utf8().data(), B_MIME_TYPE, buffer.data(), buffer.size());
            be_clipboard->Commit();
        }
        be_clipboard->Unlock();
    }

    completionHandler(0);
}

void WebClipboardProxyHaiku::read(const String& type, CompletionHandler<void(std::optional<WebCore::SharedBuffer>&&)>&& completionHandler)
{
    if (type != "text/plain" && type != "text/html") {
        completionHandler(std::nullopt);
        return;
    }

    if (be_clipboard->Lock()) {
        BMessage* data = be_clipboard->Data();
        if (data) {
            const char* text;
            ssize_t size;
            if (data->FindData(type.utf8().data(), B_MIME_TYPE, (const void**)&text, &size) == B_OK) {
                completionHandler(WebCore::SharedBuffer::create(text, size));
            } else {
                completionHandler(std::nullopt);
            }
        } else {
            completionHandler(std::nullopt);
        }
        be_clipboard->Unlock();
    } else {
        completionHandler(std::nullopt);
    }
}

void WebClipboardProxyHaiku::clear(const String&)
{
    if (be_clipboard->Lock()) {
        be_clipboard->Clear();
        be_clipboard->Commit();
        be_clipboard->Unlock();
    }
}

} // namespace WebKit
