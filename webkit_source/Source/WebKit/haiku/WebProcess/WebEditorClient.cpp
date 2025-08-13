/*
 * Copyright (C) 2013-2024 Haiku, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "WebEditorClient.h"

#include "WebPage.h"
#include "WebPageProxyMessages.h"
#include <WebCore/NotImplemented.h>
#include <WebCore/UndoStep.h>
#include <WebCore/KeyboardEvent.h>
#include <WebCore/PlatformKeyboardEvent.h>
#include <WebCore/WindowsKeyboardCodes.h>

namespace WebKit {

WebEditorClient::WebEditorClient(WebPage* page)
    : m_page(page)
{
}

WebCore::UndoStep* WebEditorClient::undoStep(uint64_t id)
{
    return m_undoSteps.get(id);
}

void WebEditorClient::addUndoStep(uint64_t id, Ref<WebCore::UndoStep> step)
{
    m_undoSteps.set(id, WTFMove(step));
}

void WebEditorClient::removeUndoStep(uint64_t id)
{
    m_undoSteps.remove(id);
}

bool WebEditorClient::shouldDeleteRange(const std::optional<WebCore::SimpleRange>&)
{
    notImplemented();
    return true;
}

bool WebEditorClient::smartInsertDeleteEnabled()
{
    notImplemented();
    return false;
}

bool WebEditorClient::isSelectTrailingWhitespaceEnabled() const
{
    notImplemented();
    return false;
}

bool WebEditorClient::isContinuousSpellCheckingEnabled()
{
    notImplemented();
    return false;
}

void WebEditorClient::toggleContinuousSpellChecking()
{
    notImplemented();
}

bool WebEditorClient::isGrammarCheckingEnabled()
{
    notImplemented();
    return false;
}

void WebEditorClient::toggleGrammarChecking()
{
    notImplemented();
}

int WebEditorClient::spellCheckerDocumentTag()
{
    notImplemented();
    return 0;
}

bool WebEditorClient::shouldBeginEditing(const WebCore::SimpleRange&)
{
    notImplemented();
    return true;
}

bool WebEditorClient::shouldEndEditing(const WebCore::SimpleRange&)
{
    notImplemented();
    return true;
}

bool WebEditorClient::shouldInsertNode(WebCore::Node&, const std::optional<WebCore::SimpleRange>&, WebCore::EditorInsertAction)
{
    notImplemented();
    return true;
}

bool WebEditorClient::shouldInsertText(const String&, const std::optional<WebCore::SimpleRange>&, WebCore::EditorInsertAction)
{
    notImplemented();
    return true;
}

bool WebEditorClient::shouldChangeSelectedRange(const std::optional<WebCore::SimpleRange>&, const std::optional<WebCore::SimpleRange>&, WebCore::Affinity, bool)
{
    notImplemented();
    return true;
}

bool WebEditorClient::shouldApplyStyle(const WebCore::StyleProperties&, const std::optional<WebCore::SimpleRange>&)
{
    notImplemented();
    return true;
}

void WebEditorClient::didApplyStyle()
{
    notImplemented();
}

bool WebEditorClient::shouldMoveRangeAfterDelete(const WebCore::SimpleRange&, const WebCore::SimpleRange&)
{
    notImplemented();
    return true;
}

void WebEditorClient::didBeginEditing()
{
    notImplemented();
}

void WebEditorClient::respondToChangedContents()
{
    notImplemented();
}

void WebEditorClient::respondToChangedSelection(WebCore::LocalFrame*)
{
    notImplemented();
}

void WebEditorClient::didEndUserTriggeredSelectionChanges()
{
    notImplemented();
}

void WebEditorClient::updateEditorStateAfterLayoutIfEditabilityChanged()
{
    notImplemented();
}

void WebEditorClient::didEndEditing()
{
    notImplemented();
}

void WebEditorClient::willWriteSelectionToPasteboard(const std::optional<WebCore::SimpleRange>&)
{
    notImplemented();
}

void WebEditorClient::didWriteSelectionToPasteboard()
{
    notImplemented();
}

void WebEditorClient::getClientPasteboardData(const std::optional<WebCore::SimpleRange>&, Vector<std::pair<String, RefPtr<WebCore::SharedBuffer>>>&)
{
    notImplemented();
}

WebCore::DOMPasteAccessResponse WebEditorClient::requestDOMPasteAccess(WebCore::DOMPasteAccessCategory, WebCore::FrameIdentifier, const String& originIdentifier)
{
    notImplemented();
    return WebCore::DOMPasteAccessResponse::DeniedForGesture;
}

void WebEditorClient::discardedComposition(const WebCore::Document&)
{
    notImplemented();
}

void WebEditorClient::canceledComposition()
{
    notImplemented();
}

void WebEditorClient::didUpdateComposition()
{
    notImplemented();
}

void WebEditorClient::registerUndoStep(WebCore::UndoStep& step)
{
    uint64_t stepID = ++m_nextUndoStepID;
    m_undoSteps.set(stepID, &step);
    m_page->send(Messages::WebPageProxy::RegisterUndoStep(stepID, step.title()));
}

void WebEditorClient::registerRedoStep(WebCore::UndoStep& step)
{
    uint64_t stepID = ++m_nextUndoStepID;
    m_undoSteps.set(stepID, &step);
    m_page->send(Messages::WebPageProxy::RegisterRedoStep(stepID, step.title()));
}

void WebEditorClient::clearUndoRedoOperations()
{
    m_undoSteps.clear();
    m_page->send(Messages::WebPageProxy::ClearUndoRedo());
}

bool WebEditorClient::canCopyCut(WebCore::LocalFrame*, bool defaultValue) const
{
    notImplemented();
    return defaultValue;
}

bool WebEditorClient::canPaste(WebCore::LocalFrame*, bool defaultValue) const
{
    notImplemented();
    return defaultValue;
}

bool WebEditorClient::canUndo() const
{
    return false;
}

bool WebEditorClient::canRedo() const
{
    return false;
}

void WebEditorClient::undo()
{
    // This is called by WebCore commands. We don't want to use it, as the
    // undo/redo state is managed in the UI process.
}

void WebEditorClient::redo()
{
    // This is called by WebCore commands. We don't want to use it, as the
    // undo/redo state is managed in the UI process.
}

void WebEditorClient::handleKeyboardEvent(WebCore::KeyboardEvent& event)
{
    const WebCore::PlatformKeyboardEvent* platformEvent = event.underlyingPlatformEvent();
    if (!platformEvent || platformEvent->type() == WebCore::PlatformEvent::Type::KeyUp)
        return;

    // TODO: Port the logic from the legacy client here.
    // For now, we will just pass the event on.
}

void WebEditorClient::handleInputMethodKeydown(WebCore::KeyboardEvent&)
{
    notImplemented();
}

void WebEditorClient::textFieldDidBeginEditing(WebCore::Element&)
{
    notImplemented();
}

void WebEditorClient::textFieldDidEndEditing(WebCore::Element&)
{
    notImplemented();
}

void WebEditorClient::textDidChangeInTextField(WebCore::Element&)
{
    notImplemented();
}

bool WebEditorClient::doTextFieldCommandFromEvent(WebCore::Element&, WebCore::KeyboardEvent*)
{
    notImplemented();
    return false;
}

void WebEditorClient::textWillBeDeletedInTextField(WebCore::Element&)
{
    notImplemented();
}

void WebEditorClient::textDidChangeInTextArea(WebCore::Element&)
{
    notImplemented();
}

void WebEditorClient::overflowScrollPositionChanged()
{
    notImplemented();
}

void WebEditorClient::subFrameScrollPositionChanged()
{
    notImplemented();
}

#if PLATFORM(COCOA)
void WebEditorClient::setInsertionPasteboard(const String& pasteboardName)
{
    notImplemented();
}
#endif

#if USE(APPKIT)
void WebEditorClient::uppercaseWord()
{
    notImplemented();
}

void WebEditorClient::lowercaseWord()
{
    notImplemented();
}

void WebEditorClient::capitalizeWord()
{
    notImplemented();
}
#endif

#if USE(AUTOMATIC_TEXT_REPLACEMENT)
void WebEditorClient::showSubstitutionsPanel(bool show)
{
    notImplemented();
}

bool WebEditorClient::substitutionsPanelIsShowing()
{
    notImplemented();
    return false;
}

void WebEditorClient::toggleSmartInsertDelete()
{
    notImplemented();
}

bool WebEditorClient::isAutomaticQuoteSubstitutionEnabled()
{
    notImplemented();
    return false;
}

void WebEditorClient::toggleAutomaticQuoteSubstitution()
{
    notImplemented();
}

bool WebEditorClient::isAutomaticLinkDetectionEnabled()
{
    notImplemented();
    return false;
}

void WebEditorClient::toggleAutomaticLinkDetection()
{
    notImplemented();
}

bool WebEditorClient::isAutomaticDashSubstitutionEnabled()
{
    notImplemented();
    return false;
}

void WebEditorClient::toggleAutomaticDashSubstitution()
{
    notImplemented();
}

bool WebEditorClient::isAutomaticTextReplacementEnabled()
{
    notImplemented();
    return false;
}

void WebEditorClient::toggleAutomaticTextReplacement()
{
    notImplemented();
}

bool WebEditorClient::isAutomaticSpellingCorrectionEnabled()
{
    notImplemented();
    return false;
}

void WebEditorClient::toggleAutomaticSpellingCorrection()
{
    notImplemented();
}
#endif

WebCore::TextCheckerClient* WebEditorClient::textChecker()
{
    notImplemented();
    return nullptr;
}

void WebEditorClient::updateSpellingUIWithGrammarString(const String&, const WebCore::GrammarDetail&)
{
    notImplemented();
}

void WebEditorClient::updateSpellingUIWithMisspelledWord(const String&)
{
    notImplemented();
}

void WebEditorClient::showSpellingUI(bool)
{
    notImplemented();
}

bool WebEditorClient::spellingUIIsShowing()
{
    notImplemented();
    return false;
}

void WebEditorClient::setInputMethodState(WebCore::Element*)
{
    notImplemented();
}

bool WebEditorClient::performTwoStepDrop(WebCore::DocumentFragment&, const WebCore::SimpleRange&, bool)
{
    notImplemented();
    return false;
}

} // namespace WebKit
