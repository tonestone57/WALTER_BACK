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

#include "WebPageProxy.h"
#include <memory>

namespace WebKit {

class WebView;

#include "PageConfiguration.h"
#include "IconLoadingClientHaiku.h"
#include "FindClientHaiku.h"
#include "FindOptions.h"
#include "WebContextMenuProxy.h"
#include "ContextMenuContextData.h"
#include "WebOpenPanelResultListenerProxy.h"
#include "FileChooserSettings.h"
#include <Handler.h>

namespace WebCore {
    class UndoStep;
}

class WebContextMenuProxy;

struct UndoStepInfo {
    uint64_t id;
    String title;
};

class WebPageProxyHaiku final : public WebPageProxy, public BHandler {
public:
    static Ref<WebPageProxy> create(PageConfiguration&);
    ~WebPageProxyHaiku();

    WebView* view() const;

    void loadURL(const String&);
    void reload();
    void goBack();
    void goForward();
    void stopLoading();

    void setViewNeedsDisplay(const WebCore::Region&);

    const String& mainFrameTitle() const;
    const String& mainFrameURL() const;
    double estimatedProgress() const;
    WebCore::Image* favicon() const;

    void increaseZoomFactor(bool textOnly);
    void decreaseZoomFactor(bool textOnly);
    void resetZoomFactor();

    void findString(const String&, OptionSet<FindOptions>, unsigned maxMatchCount);

    bool canUndo() const;
    bool canRedo() const;

    void undo();
    void redo();

private:
    // WebPageProxy
    void showContextMenu(FrameInfoData&&, ContextMenuContextData&&, const UserData&) override;
    void runOpenPanel(WebFrameProxy&, FrameInfoData&&, API::OpenPanelParameters&, WebOpenPanelResultListenerProxy&) override;
    void runJavaScriptAlert(WebFrameProxy&, FrameInfoData&&, const String&, CompletionHandler<void()>&&) override;
    void runJavaScriptConfirm(WebFrameProxy&, FrameInfoData&&, const String&, CompletionHandler<void(bool)>&&) override;
    void runJavaScriptPrompt(WebFrameProxy&, FrameInfoData&&, const String&, const String&, CompletionHandler<void(const String&)>&&) override;
    void runAuthenticationPanel(WebFrameProxy&, FrameInfoData&&, WebCore::AuthenticationChallenge&, CompletionHandler<void(WebCore::Credential, WebCore::ShouldContinueWithoutCredential)>&&) override;
    RefPtr<WebPageProxy> createNewPage(WebCore::WindowFeatures&&) override;
    Ref<WebPageProxy> createInspectorPage() override;
    void createView();

    // BHandler
    void MessageReceived(BMessage* message) override;

    // IPC::MessageReceiver
    void didReceiveMessage(IPC::Connection&, IPC::Decoder&) override;

    void setWindowRect(const WebCore::FloatRect&);
    void getWindowRect(CompletionHandler<void(WebCore::FloatRect)>&&);

    void registerUndoStep(uint64_t, const String&);
    void registerRedoStep(uint64_t, const String&);
    void clearUndoRedo();

    // WebPageProxy
    void didUpdateBackForwardList(WebFrameProxy*, API::BackForwardListItem*, const Vector<Ref<API::BackForwardListItem>>&, const Vector<Ref<API::BackForwardListItem>>&) override;
    void didFinishLoadForFrame(WebCore::FrameIdentifier, FrameInfoData&&, WebCore::ResourceRequest&&, std::optional<WebCore::NavigationIdentifier>, bool, const UserData&) override;
    void didReceiveTitleForFrame(WebCore::FrameIdentifier, const String&, const UserData&) override;
    void didCommitLoadForFrame(WebCore::FrameIdentifier, FrameInfoData&&, WebCore::ResourceRequest&&, std::optional<WebCore::NavigationIdentifier>, String&&, bool, WebCore::FrameLoadType, const WebCore::CertificateInfo&, bool, String&&, WebCore::ResourceResponseSource, bool, WebCore::HasInsecureContent, WebCore::MouseEventPolicy, const UserData&) override;
    void didStartProgress() override;
    void didChangeProgress(double) override;
    void didFinishProgress() override;

    std::unique_ptr<WebView> m_view;
    String m_mainFrameTitle;
    String m_mainFrameURL;
    double m_estimatedProgress;
    std::unique_ptr<IconLoadingClientHaiku> m_iconLoadingClient;
    std::unique_ptr<FindClientHaiku> m_findClient;
    RefPtr<WebContextMenuProxy> m_activeContextMenu;
    RefPtr<WebOpenPanelResultListenerProxy> m_openPanelResultListener;

    // WebContextMenuProxy::Client
    void contextMenuItemSelected(const WebContextMenuItemData&) override;

    Vector<UndoStepInfo> m_undoStack;
    Vector<UndoStepInfo> m_redoStack;
};

} // namespace WebKit
