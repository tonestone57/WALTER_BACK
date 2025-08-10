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
#include "WebPageProxy.h"

#include "APIView.h"
#include "DrawingAreaProxy.h"
#include "NativeWebMouseEvent.h"
#include "WebPageCreationParameters.h"
#include "WebPageProxyHaiku.h"
#include <WebCore/NotImplemented.h>

#include <Message.h>

namespace WebKit {

void WebPageProxy::platformInitialize()
{
    // Haiku-specific initialization.
}

void WebPageProxy::getPlatformEditorState(WebCore::EditorState& state) const
{
    notImplemented();
}

Ref<API::View> WebPageProxy::createView()
{
    return API::View::create(*this);
}

void WebPageProxy::setView(API::View* view)
{
    m_view = view;
    if (m_view)
        m_view->setClient(makeUnique<ViewClient>());
}

void WebPageProxy::makeViewBlank()
{
    if (m_view)
        m_view->makeBlank();
}

void WebPageProxy::setDrawingArea(std::unique_ptr<DrawingAreaProxy>&& drawingArea)
{
    if (drawingArea)
        drawingArea->setShouldScaleViewToFitDocument(true);
    m_drawingArea = WTFMove(drawingArea);
}

void ViewClient::MessageReceived(BMessage* message)
{
    switch (message->what) {
    case 'wups': // HANDLE_SHUTDOWN
        // NOTE: This message never arrives here when the BApplication is already
        // processing B_QUIT_REQUESTED. Then the view will be detached and instruct
        // the BWebPage handler to shut itself down, but BApplication will not
        // process additional messages. That's why the windows containing WebViews
        // are detaching the views already in their QuitRequested() hooks and
        // LauncherApp calls these hooks already in its own QuitRequested() hook.
        Looper()->RemoveHandler(this);
        delete this;
        // TOAST!
        return;
    case 'lurl': // HANDLE_LOAD_URL
        notImplemented();
        break;
    case 'reld': // HANDLE_RELOAD:
	notImplemented();
	break;
    case 'back': // HANDLE_GO_BACK:
        notImplemented();
        break;
    case 'fwrd': // HANDLE_GO_FORWARD:
        notImplemented();
        break;
    case 'stop': // HANDLE_STOP_LOADING:
        notImplemented();
        break;

    case 'vsbl': // HANDLE_SET_VISIBLE:
        notImplemented();
        break;

    case 'draw': // HANDLE_DRAW: {
        notImplemented();
        break;
    }
    case 'rszd': // HANDLE_FRAME_RESIZED:
        notImplemented();
        break;

    case 'focs': // HANDLE_FOCUSED:
        notImplemented();
        break;
    case 'actd': // HANDLE_ACTIVATED:
        notImplemented();
        break;

    case B_MOUSE_MOVED:
        // fall through
    case B_MOUSE_DOWN:
    case B_MOUSE_UP:
        notImplemented();
        break;
    case B_MOUSE_WHEEL_CHANGED:
        notImplemented();
        break;
    case B_KEY_DOWN:
    case B_KEY_UP:
        notImplemented();
        break;

	case 'zmfr': // HANDLE_CHANGE_ZOOM_FACTOR:
		notImplemented();
		break;
    case 'find': // HANDLE_FIND_STRING:
        notImplemented();
        break;

	case 'stsm': // HANDLE_SET_STATUS_MESSAGE: {
		notImplemented();
		break;
	}

    case 'rsnt': // HANDLE_RESEND_NOTIFICATIONS:
        notImplemented();
        break;
    case 'sedc': // HANDLE_SEND_EDITING_CAPABILITIES:
        notImplemented();
        break;
    case 'spsc': // HANDLE_SEND_PAGE_SOURCE:
        notImplemented();
        break;

    case 'wupt': // HANDLE_WARM_UP_TILE:
        notImplemented();
        break;

    case 'dkay': // HANDLE_DECAY_ACCESS_COUNT:
        notImplemented();
        break;

    case 'ctup': // HANDLE_COMPRESSION_TIER_UP:
        notImplemented();
        break;

    case B_REFS_RECEIVED: {
        notImplemented();
	break;
    }
    case B_CANCEL: {
        notImplemented();
	break;
    }

    default:
        BHandler::MessageReceived(message);
    }
}


} // namespace WebKit
