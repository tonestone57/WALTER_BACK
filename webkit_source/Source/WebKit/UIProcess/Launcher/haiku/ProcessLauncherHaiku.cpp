/*
 * Copyright (C) 2024 Your Name <you@example.com>
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
#include "ProcessLauncher.h"

#include "Connection.h"
#include "ProcessExecutablePath.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <wtf/text/CString.h>
#include <wtf/UniStdExtras.h>

#include <support/Locker.h>
#include <app/Roster.h>
#include <kernel/image.h>

namespace WebKit {

void ProcessLauncher::launchProcess()
{
    IPC::Connection::SocketPair socketPair = IPC::Connection::createPlatformConnection();

    String executablePath = processExecutablePath();
    if (executablePath.isEmpty()) {
        printf("Could not find the WebProcess executable\n");
        return;
    }

    CString executablePathCString = executablePath.utf8();
    const char* argv[] = {
        executablePathCString.data(),
        "--" WEBKIT_PROCESS_NAME_PREFIX "process",
        String::number(m_launchOptions.processIdentifier.toUInt64()).utf8().data(),
        String::number(socketPair.server).utf8().data(),
        nullptr
    };

    thread_id thread = load_image_etc(3, argv, (const char**)environ, B_NORMAL_PRIORITY,
        B_CURRENT_TEAM, 0);

    if (thread < B_OK) {
        printf("Failed to launch %s: %s\n", executablePath.utf8().data(), strerror(thread));
        return;
    }

    m_processID = thread;
    resume_thread(thread);

    didFinishLaunchingProcess(m_processID, IPC::Connection::Identifier(socketPair.client));
}

void ProcessLauncher::terminateProcess()
{
    if (!m_processID)
        return;

    kill_thread(m_processID);
    m_processID = 0;
}

void ProcessLauncher::platformInvalidate()
{
}

} // namespace WebKit
