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
#include "WebPageProxyHaiku.h"

#include "WebView.h"

namespace WebKit {

Ref<WebPageProxy> WebPageProxyHaiku::create(PageConfiguration& configuration)
{
    return adoptRef(*new WebPageProxyHaiku(configuration));
}

WebPageProxyHaiku::WebPageProxyHaiku(PageConfiguration& configuration)
    : WebPageProxy(configuration)
    , m_iconLoadingClient(std::make_unique<IconLoadingClientHaiku>())
    , m_findClient(std::make_unique<FindClientHaiku>())
{
    createView();
    setIconLoadingClient(m_iconLoadingClient.get());
    setFindClient(m_findClient.get());
}

WebPageProxyHaiku::~WebPageProxyHaiku()
{
}

WebView* WebPageProxyHaiku::view() const
{
    return m_view.get();
}

#include <WebCore/Region.h>

void WebPageProxyHaiku::createView()
{
    m_view = std::make_unique<WebView>(*this);
}

#include <WebCore/ResourceRequest.h>

void WebPageProxyHaiku::setViewNeedsDisplay(const WebCore::Region& region)
{
    if (m_view)
        m_view->Invalidate(region.bounds());
}

const String& WebPageProxyHaiku::mainFrameTitle() const
{
    return m_mainFrameTitle;
}

#include <WebCore/MessageLevel.h>
#include <WebCore/MessageSource.h>

void WebPageProxyHaiku::didReceiveMessage(IPC::Connection& connection, IPC::Decoder& decoder)
{
    if (decoder.messageName() == Messages::WebPageProxy::AddMessageToConsole::name()) {
        IPC::handleMessage<Messages::WebPageProxy::AddMessageToConsole>(connection, decoder, this, &WebPageProxyHaiku::addMessageToConsole);
        return;
    }

    WebPageProxy::didReceiveMessage(connection, decoder);
}

#include <TextCheck.h>
#include <Url.h>

void WebPageProxyHaiku::addMessageToConsole(WebCore::MessageSource source, WebCore::MessageLevel level, const String& message, uint64_t lineNumber, uint64_t columnNumber, const String& sourceID)
{
    fprintf(stderr, "JS: %s:%" PRIu64 ":%" PRIu64 " %s\n", sourceID.utf8().data(), lineNumber, columnNumber, message.utf8().data());
}

void WebPageProxyHaiku::launchURL(const URL& url)
{
    // This is the proper way to ask the system to open a URL, which will
    // typically be handled by the Tracker.
    BUrl burl(url);
    burl.OpenWithPreferredApplication();
}

void WebPageProxyHaiku::checkSpellingOfString(const String& text, CompletionHandler<void(int32_t, int32_t)>&& completionHandler)
{
    int32_t misspellingOffset = -1;
    int32_t misspellingLength = 0;
    if (gTextCheck)
        gTextCheck->FindMisspelledWord(text.utf8().data(), &misspellingOffset, &misspellingLength);
    completionHandler(misspellingOffset, misspellingLength);
}

void WebPageProxyHaiku::checkGrammarOfString(const String& text, CompletionHandler<void(Vector<WebCore::GrammarDetail>, int32_t, int32_t)>&& completionHandler)
{
    Vector<WebCore::GrammarDetail> details;
    int32_t badGrammarOffset = -1;
    int32_t badGrammarLength = 0;
    // Not implemented in Haiku's BTextCheck
    completionHandler(details, badGrammarOffset, badGrammarLength);
}

void WebPageProxyHaiku::getGuessesForWord(const String& word, const String& context, CompletionHandler<void(Vector<String>)>&& completionHandler)
{
    Vector<String> guesses;
    if (gTextCheck) {
        BStringList suggestions;
        gTextCheck->GetGuesses(word.utf8().data(), &suggestions);
        for (int32 i = 0; i < suggestions.CountStrings(); i++)
            guesses.append(String::fromUTF8(suggestions.StringAt(i).String()));
    }
    completionHandler(guesses);
}

#include "FrameInfoData.h"

#include "BWebView.h"
#include "BWebPageClient.h"
#include "EditorState.h"

#include <Window.h>

void WebPageProxyHaiku::setWindowRect(const WebCore::FloatRect& rect)
{
    if (auto* webView = static_cast<BWebView*>(view()->Parent())) {
        if (BWindow* window = webView->Window()) {
            // BWindow::MoveTo and ::ResizeTo expect top-left corner and width/height,
            // but WebCore::FloatRect is a rectangle.
            window->MoveTo(rect.x(), rect.y());
            window->ResizeTo(rect.width(), rect.height());
        }
    }
}

void WebPageProxyHaiku::getWindowRect(CompletionHandler<void(WebCore::FloatRect)>&& completionHandler)
{
    WebCore::FloatRect rect;
    if (auto* webView = static_cast<BWebView*>(view()->Parent())) {
        if (BWindow* window = webView->Window())
            rect = WebCore::FloatRect(window->Frame());
    }
    completionHandler(rect);
}

void WebPageProxyHaiku::setToolbarsVisible(bool visible)
{
    if (auto* webView = static_cast<BWebView*>(view()->Parent()))
        if (auto* client = webView->Client())
            client->SetToolBarsVisible(visible, webView);
}

void WebPageProxyHaiku::toolbarsAreVisible(CompletionHandler<void(bool)>&& completionHandler)
{
    bool areVisible = true;
    if (auto* webView = static_cast<BWebView*>(view()->Parent()))
        if (auto* client = webView->Client())
            areVisible = client->AreToolBarsVisible(webView);
    completionHandler(areVisible);
}

void WebPageProxyHaiku::setStatusbarVisible(bool visible)
{
    if (auto* webView = static_cast<BWebView*>(view()->Parent()))
        if (auto* client = webView->Client())
            client->SetStatusBarVisible(visible, webView);
}

void WebPageProxyHaiku::statusbarIsVisible(CompletionHandler<void(bool)>&& completionHandler)
{
    bool isVisible = true;
    if (auto* webView = static_cast<BWebView*>(view()->Parent()))
        if (auto* client = webView->Client())
            isVisible = client->IsStatusBarVisible(webView);
    completionHandler(isVisible);
}

void WebPageProxyHaiku::updateUndoRedoState()
{
    if (auto* webView = static_cast<BWebView*>(view()->Parent()))
        if (auto* client = webView->Client())
            client->UndoRedoStateChanged(webView);
}

void WebPageProxyHaiku::registerUndoStep(uint64_t id, const String& title)
{
    m_undoStack.append({ id, title });
    m_redoStack.clear();
    updateUndoRedoState();
}

void WebPageProxyHaiku::registerRedoStep(uint64_t id, const String& title)
{
    m_redoStack.append({ id, title });
    updateUndoRedoState();
}

void WebPageProxyHaiku::clearUndoRedo()
{
    m_undoStack.clear();
    m_redoStack.clear();
    updateUndoRedoState();
}

void WebPageProxyHaiku::didReceiveTitleForFrame(WebCore::FrameIdentifier frameID, const String& title, const UserData&)
{
    if (mainFrame() && mainFrame()->frameID() == frameID) {
        m_mainFrameTitle = title;
        if (auto* webView = static_cast<BWebView*>(view()->Parent())) {
            if (auto* client = webView->Client())
                client->TitleChanged(title, webView);
        }
    }
}

Ref<WebPageProxy> WebPageProxyHaiku::createInspectorPage()
{
    if (auto* webView = static_cast<BWebView*>(view()->Parent())) {
        if (auto* client = webView->Client()) {
            if (auto* inspectorView = client->CreateInspectorWindow())
                return inspectorView->page();
        }
    }

    return WebPageProxy::createInspectorPage();
}

void WebPageProxyHaiku::createNewPage(WebCore::WindowFeatures windowFeatures, const WebCore::ResourceRequest& request, CompletionHandler<void(std::optional<WebKit::WebPageProxyIdentifier>)>&& completionHandler)
{
    auto newPage = createNewPage(WTFMove(windowFeatures));
    if (!newPage) {
        completionHandler(std::nullopt);
        return;
    }

    newPage->loadRequest(request);
    completionHandler(newPage->identifier());
}

RefPtr<WebPageProxy> WebPageProxyHaiku::createNewPage(WebCore::WindowFeatures&& features)
{
    if (auto* webView = static_cast<BWebView*>(view()->Parent())) {
        if (auto* client = webView->Client()) {
            BRect windowFrame;
            if (features.x || features.y || features.width || features.height) {
                if (BWindow* window = webView->Window())
                    windowFrame = window->Frame().OffsetByCopy(10, 10);
            }

            if (features.x)
                windowFrame.OffsetTo(*features.x, windowFrame.top);
            if (features.y)
                windowFrame.OffsetTo(windowFrame.left, *features.y);
            if (features.width)
                windowFrame.right = windowFrame.left + *features.width - 1;
            if (features.height)
                windowFrame.bottom = windowFrame.top + *features.height - 1;

            client->NewPageCreated(nullptr, windowFrame,
                features.dialog.value_or(false),
                features.resizable.value_or(true),
                true /* activate */);

            // FIXME: The client should return the new view, but the hook is void.
            // This requires a larger refactoring of the client interface.
            // For now, we assume the last created page is the one we want.
            if (auto* newView = client->LastCreatedWindow())
                 return &newView->page();
        }
    }
    return nullptr;
}

void WebPageProxyHaiku::didUpdateBackForwardList(WebFrameProxy*, API::BackForwardListItem*, const Vector<Ref<API::BackForwardListItem>>&, const Vector<Ref<API::BackForwardListItem>>&)
{
    if (auto* webView = static_cast<BWebView*>(view()->Parent())) {
        if (auto* client = webView->Client())
            client->NavigationCapabilitiesChanged(canGoBack(), canGoForward(), isLoading(), webView);
    }
}

void WebPageProxyHaiku::didFinishLoadForFrame(WebCore::FrameIdentifier frameID, FrameInfoData&& frameInfo, WebCore::ResourceRequest&& request, std::optional<WebCore::NavigationIdentifier> navigationID, bool wasLoadedFromBackForwardCache, const UserData& userData)
{
    WebPageProxy::didFinishLoadForFrame(frameID, WTFMove(frameInfo), WTFMove(request), navigationID, wasLoadedFromBackForwardCache, userData);

    if (mainFrame() && mainFrame()->frameID() == frameID) {
        if (auto* webView = static_cast<BWebView*>(view()->Parent())) {
            if (auto* client = webView->Client())
                client->LoadFinished(m_mainFrameURL, webView);
        }
    }
}

const String& WebPageProxyHaiku::mainFrameURL() const
{
    return m_mainFrameURL;
}

void WebPageProxyHaiku::didCommitLoadForFrame(WebCore::FrameIdentifier frameID, FrameInfoData&& frameInfo, WebCore::ResourceRequest&& request, std::optional<WebCore::NavigationIdentifier> navigationID, String&& mimeType, bool frameHasCustomContentProvider, WebCore::FrameLoadType frameLoadType, const WebCore::CertificateInfo& certificateInfo, bool usedLegacyTLS, String&& proxyName, WebCore::ResourceResponseSource source, bool containsPluginDocument, WebCore::HasInsecureContent hasInsecureContent, WebCore::MouseEventPolicy mouseEventPolicy, const UserData& userData)
{
    WebPageProxy::didCommitLoadForFrame(frameID, WTFMove(frameInfo), WTFMove(request), navigationID, WTFMove(mimeType), frameHasCustomContentProvider, frameLoadType, certificateInfo, usedLegacyTLS, WTFMove(proxyName), source, containsPluginDocument, hasInsecureContent, mouseEventPolicy, userData);

    if (mainFrame() && mainFrame()->frameID() == frameID) {
        m_mainFrameURL = frameInfo.url;
        if (auto* webView = static_cast<BWebView*>(view()->Parent())) {
            if (auto* client = webView->Client())
                client->LoadCommitted(m_mainFrameURL, webView);
        }
    }
}

double WebPageProxyHaiku::estimatedProgress() const
{
    return m_estimatedProgress;
}

void WebPageProxyHaiku::didStartProgress()
{
    m_estimatedProgress = 0.0;
    if (auto* webView = static_cast<BWebView*>(view()->Parent())) {
        if (auto* client = webView->Client())
            client->LoadNegotiating(mainFrameURL(), webView);
    }
}

void WebPageProxyHaiku::didFailLoad(WebCore::FrameIdentifier frameID, FrameInfoData&& frameInfo, WebCore::ResourceRequest&&, const WebCore::ResourceError& error)
{
    if (mainFrame() && mainFrame()->frameID() == frameID) {
        if (auto* webView = static_cast<BWebView*>(view()->Parent())) {
            if (auto* client = webView->Client())
                client->LoadFailed(frameInfo.url, webView);
        }
    }
}

void WebPageProxyHaiku::didChangeProgress(double value)
{
    m_estimatedProgress = value;
    if (auto* webView = static_cast<BWebView*>(view()->Parent())) {
        if (auto* client = webView->Client())
            client->LoadProgress(value, webView);
    }
}

void WebPageProxyHaiku::didFinishProgress()
{
    m_estimatedProgress = 1.0;
}

void WebPageProxyHaiku::loadURL(const String& url)
{
    loadRequest(WebCore::ResourceRequest(URL(url)));
}

void WebPageProxyHaiku::reload()
{
    WebPageProxy::reload(false);
}

void WebPageProxyHaiku::goBack()
{
    WebPageProxy::goBack();
}

void WebPageProxyHaiku::goForward()
{
    WebPageProxy::goForward();
}

void WebPageProxyHaiku::stopLoading()
{
    WebPageProxy::stopLoading();
}

WebCore::Image* WebPageProxyHaiku::favicon() const
{
    return m_iconLoadingClient->favicon();
}

void WebPageProxyHaiku::increaseZoomFactor(bool textOnly)
{
    if (textOnly)
        setTextZoomFactor(textZoomFactor() * 1.2);
    else
        setPageZoomFactor(pageZoomFactor() * 1.2);
}

void WebPageProxyHaiku::decreaseZoomFactor(bool textOnly)
{
    if (textOnly)
        setTextZoomFactor(textZoomFactor() / 1.2);
    else
        setPageZoomFactor(pageZoomFactor() / 1.2);
}

void WebPageProxyHaiku::resetZoomFactor()
{
    setTextZoomFactor(1.0);
    setPageZoomFactor(1.0);
}

#include "WebContextMenuProxyHaiku.h"

#include <FilePanel.h>

void WebPageProxyHaiku::findString(const String& string, OptionSet<FindOptions> options, unsigned maxMatchCount)
{
    WebPageProxy::findString(string, options, maxMatchCount);
}

bool WebPageProxyHaiku::canUndo() const
{
    return !m_undoStack.isEmpty();
}

bool WebPageProxyHaiku::canRedo() const
{
    return !m_redoStack.isEmpty();
}

void WebPageProxyHaiku::undo()
{
    if (m_undoStack.isEmpty())
        return;

    UndoStepInfo step = m_undoStack.takeLast();
    send(Messages::WebPage::Undo(step.id));
}

void WebPageProxyHaiku::redo()
{
    if (m_redoStack.isEmpty())
        return;

    UndoStepInfo step = m_redoStack.takeLast();
    send(Messages::WebPage::Redo(step.id));
}

#include <Entry.h>
#include <Path.h>

void WebPageProxyHaiku::runOpenPanel(WebFrameProxy&, FrameInfoData&&, API::OpenPanelParameters& parameters, WebOpenPanelResultListenerProxy& listener)
{
    m_openPanelResultListener = &listener;
    BFilePanel* panel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), nullptr, 0, parameters.allowMultipleFiles());
    panel->Show();
}

void WebPageProxyHaiku::MessageReceived(BMessage* message)
{
    switch (message->what) {
    case B_REFS_RECEIVED: {
        entry_ref ref;
        Vector<String> files;
        for (int32 i = 0; message->FindRef("refs", i, &ref) == B_OK; i++) {
            BPath path(&ref);
            files.append(path.Path());
        }
        m_openPanelResultListener->didChooseFiles(files);
        m_openPanelResultListener = nullptr;
        break;
    }
    case B_CANCEL:
        m_openPanelResultListener->didCancel();
        m_openPanelResultListener = nullptr;
        break;
    default:
        BHandler::MessageReceived(message);
    }
}

#include <Alert.h>

void WebPageProxyHaiku::showContextMenu(FrameInfoData&& frameInfo, ContextMenuContextData&& contextMenuContext, const UserData& userData)
{
    auto contextMenu = WebContextMenuProxyHaiku::create(*this, WTFMove(contextMenuContext), userData);
    m_activeContextMenu = contextMenu.ptr();
    contextMenu->show();
}

void WebPageProxyHaiku::runJavaScriptAlert(WebFrameProxy&, FrameInfoData&&, const String& message, CompletionHandler<void()>&& completionHandler)
{
    BAlert* alert = new BAlert("JavaScript Alert", message.utf8().data(), "OK");
    alert->Go(nullptr);
    completionHandler();
}

#include "JSPromptPanel.h"

void WebPageProxyHaiku::runJavaScriptConfirm(WebFrameProxy&, FrameInfoData&&, const String& message, CompletionHandler<void(bool)>&& completionHandler)
{
    BAlert* alert = new BAlert("JavaScript Confirm", message.utf8().data(), "Cancel", "OK");
    int32 result = alert->Go();
    completionHandler(result == 1);
}

#include <AuthenticationChallenge.h>
#include <Credential.h>

void WebPageProxyHaiku::runAuthenticationPanel(WebFrameProxy&, FrameInfoData&&, WebCore::AuthenticationChallenge& challenge, CompletionHandler<void(WebCore::Credential, WebCore::ShouldContinueWithoutCredential)>&& completionHandler)
{
    if (auto* webView = static_cast<BWebView*>(view()->Parent())) {
        if (auto* client = webView->Client()) {
            BString message = challenge.localizedDescription();
            BString user;
            BString password;
            bool remember = false;
            if (client->AuthenticationChallenge(message, user, password, remember)) {
                completionHandler(WebCore::Credential(user.String(), password.String(), WebCore::Credential::Persistence::None), WebCore::ShouldContinueWithoutCredential::No);
                return;
            }
        }
    }
    completionHandler({}, WebCore::ShouldContinueWithoutCredential::Yes);
}

void WebPageProxyHaiku::runJavaScriptPrompt(WebFrameProxy&, FrameInfoData&&, const String& message, const String& defaultValue, CompletionHandler<void(const String&)>&& completionHandler)
{
    JSPromptPanel* panel = new JSPromptPanel("JavaScript Prompt", message.utf8().data(), defaultValue.utf8().data(), WTFMove(completionHandler));
    panel->Show();
}

void WebPageProxyHaiku::contextMenuItemSelected(const WebContextMenuItemData& item)
{
    if (item.action() == static_cast<WebCore::ContextMenuAction>(WebCore::ContextMenuAction::LastAPIAction + 1)) {
        if (m_activeContextMenu) {
            const URL& linkURL = m_activeContextMenu->context().linkURL();
            if (!linkURL.isEmpty()) {
                if (auto* webView = static_cast<BWebView*>(view()->Parent())) {
                    if (auto* client = webView->Client())
                        client->NewWindowRequested(linkURL.string(), true);
                }
            }
        }
        return;
    }

    if (item.action() == static_cast<WebCore::ContextMenuAction>(WebCore::ContextMenuAction::LastAPIAction + 2)) {
        if (m_activeContextMenu) {
            const URL& linkURL = m_activeContextMenu->context().linkURL();
            if (!linkURL.isEmpty())
                send(Messages::WebPage::DownloadFile(linkURL, { }));
        }
        return;
    }

    if (item.action() == ContextMenuAction::Ignore)
        return;

    if (item.action() < ContextMenuAction::LastAPIAction) {
         WebPageProxy::contextMenuItemSelected(item);
        return;
    }

    switch (item.action()) {
    case ContextMenuAction::DownloadFile:
        send(Messages::WebPage::DownloadFile(item.url(), item.title()));
        break;
    case ContextMenuAction::DownloadImage:
        send(Messages::WebPage::DownloadImage(item.url()));
        break;
    case ContextMenuAction::CopyImage:
        send(Messages::WebPage::CopyImage(item.url()));
        break;
    case ContextMenuAction::CopyImageURL:
        send(Messages::WebPage::CopyImageURL(item.url()));
        break;
    case ContextMenuAction::CopyMediaURL:
        send(Messages::WebPage::CopyMediaURL(item.url()));
        break;
    case ContextMenuAction::OpenImageInNewWindow:
        send(Messages::WebPage::OpenImageInNewWindow(item.url()));
        break;
    case ContextMenuAction::OpenMediaInNewWindow:
        send(Messages::WebPage::OpenMediaInNewWindow(item.url()));
        break;
    case ContextMenuAction::ToggleMediaControls:
        send(Messages::WebPage::ToggleMediaControls(item.url()));
        break;
    case ContextMenuAction::ToggleMediaLoop:
        send(Messages::WebPage::ToggleMediaLoop(item.url()));
        break;
    default:
        break;
    }
}

} // namespace WebKit
