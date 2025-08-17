/*
 * Copyright (C) 2019 Haiku, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS AS IS''
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
#include "WebProcessPool.h"

#include "GPUProcessCreationParameters.h"
#include "WebProcessCreationParameters.h"
#include <WebCore/NotImplemented.h>

#include <Application.h>
#include <Path.h>
#include <Roster.h>

namespace WebKit {

void WebProcessPool::platformInitialize(NeedsGlobalStaticInitialization)
{
}

void WebProcessPool::platformInitializeGPUProcess(GPUProcessCreationParameters& parameters)
{
    // Assume the GPUProcess executable is in the same directory as the main application executable.
    app_info info;
    if (be_app->GetAppInfo(&info) != B_OK)
        return;

    BPath path(&info.ref);
    path.GetParent(&path);
    path.Append("GPUProcess");
    parameters.processPath = path.Path();
    printf("GPUProcess executable path: %s\n", parameters.processPath.utf8().data());
}

void WebProcessPool::platformInitializeNetworkProcess(NetworkProcessCreationParameters& parameters)
{
    // Assume the NetworkProcess executable is in the same directory as the main application executable.
    app_info info;
    if (be_app->GetAppInfo(&info) != B_OK)
        return;

    BPath path(&info.ref);
    path.GetParent(&path);
    path.Append("NetworkProcess");
    parameters.processPath = path.Path();
    printf("NetworkProcess executable path: %s\n", parameters.processPath.utf8().data());
}

void WebProcessPool::platformInitializeWebProcess(const WebKit::WebProcessProxy&, WebProcessCreationParameters& parameters)
{
    // Assume the WebProcess executable is in the same directory as the main application executable.
    app_info info;
    be_app->GetAppInfo(&info);
    BPath path(&info.ref);
    path.GetParent(&path);
    path.Append("WebProcess");
    parameters.processPath = path.Path();
    printf("WebProcess executable path: %s\n", parameters.processPath.utf8().data());
}

void WebProcessPool::platformInvalidateContext()
{
}

void WebProcessPool::platformResolvePathsForSandboxExtensions()
{
    // FIXME: This needs to be implemented to set up the sandbox for the
    // auxiliary processes. This will require a deep understanding of the
    // Haiku security model and the available sandboxing APIs.
}

} // namespace WebKit
