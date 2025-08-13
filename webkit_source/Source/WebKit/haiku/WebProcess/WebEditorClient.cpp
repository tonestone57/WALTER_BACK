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
#include <WebCore/Editor.h>

namespace WebKit {

WebEditorClient::WebEditorClient(WebPage* page)
    : m_page(page)
{
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
    if (!m_isInRedo)
        m_redoStack.clear();
    m_undoStack.append(&step);
    m_page->send(Messages::WebPageProxy::UndoStateChanged(canUndo(), canRedo()));
}

void WebEditorClient::registerRedoStep(WebCore::UndoStep& step)
{
    m_redoStack.append(&step);
    m_page->send(Messages::WebPageProxy::UndoStateChanged(canUndo(), canRedo()));
}

void WebEditorClient::clearUndoRedoOperations()
{
    m_undoStack.clear();
    m_redoStack.clear();
    m_page->send(Messages::WebPageProxy::UndoStateChanged(false, false));
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
    return !m_undoStack.isEmpty();
}

bool WebEditorClient::canRedo() const
{
    return !m_redoStack.isEmpty();
}

void WebEditorClient::undo()
{
    if (canUndo()) {
        RefPtr<WebCore::UndoStep> step = m_undoStack.takeLast();
        step->unapply();
    }
}

void WebEditorClient::redo()
{
    if (canRedo()) {
        RefPtr<WebCore::UndoStep> step = m_redoStack.takeLast();
        m_isInRedo = true;
        step->reapply();
        m_isInRedo = false;
    }
}

#include <WebCore/FocusController.h>
#include <WebCore/FrameSelection.h>
#include <WebCore/Page.h>

void WebEditorClient::handleKeyboardEvent(WebCore::KeyboardEvent& event)
{
    const WebCore::PlatformKeyboardEvent* platformEvent = event.underlyingPlatformEvent();
    if (!platformEvent || platformEvent->type() == WebCore::PlatformEvent::Type::KeyUp)
        return;

    WebCore::LocalFrame* frame = m_page->corePage()->focusController().focusedOrMainFrame();
    if (!frame || !frame->document())
        return;

    if (!frame->selection().isRange() && !frame->editor().canEdit())
        return;

    bool handled = true;
    switch (platformEvent->windowsVirtualKeyCode()) {
    case VK_BACK:
        frame->editor().deleteWithDirection(WebCore::SelectionDirection::Backward,
            platformEvent->controlKey() ? WebCore::TextGranularity::WordGranularity
                : WebCore::TextGranularity::CharacterGranularity,
            false, true);
        break;
    case VK_DELETE:
        frame->editor().deleteWithDirection(WebCore::SelectionDirection::Forward,
            platformEvent->controlKey() ? WebCore::TextGranularity::WordGranularity
                : WebCore::TextGranularity::CharacterGranularity,
            false, true);
        break;
    case VK_LEFT:
        frame->selection().modify(platformEvent->shiftKey()
                ? WebCore::FrameSelection::Alteration::Extend : WebCore::FrameSelection::Alteration::Move,
            WebCore::SelectionDirection::Left,
            platformEvent->controlKey() ? WebCore::TextGranularity::WordGranularity
                : WebCore::TextGranularity::CharacterGranularity,
            WebCore::UserTriggered::Yes);
        break;
    case VK_RIGHT:
        frame->selection().modify(platformEvent->shiftKey() ? WebCore::FrameSelection::Alteration::Extend
                : WebCore::FrameSelection::Alteration::Move,
            WebCore::SelectionDirection::Right,
            platformEvent->controlKey() ? WebCore::TextGranularity::WordGranularity
                : WebCore::TextGranularity::CharacterGranularity,
            WebCore::UserTriggered::Yes);
        break;
    case VK_UP:
        frame->selection().modify(platformEvent->shiftKey() ? WebCore::FrameSelection::Alteration::Extend
                : WebCore::FrameSelection::Alteration::Move,
            WebCore::SelectionDirection::Backward,
            platformEvent->controlKey() ? WebCore::TextGranularity::ParagraphGranularity
                : WebCore::TextGranularity::LineGranularity,
            WebCore::UserTriggered::Yes);
        break;
    case VK_DOWN:
        frame->selection().modify(platformEvent->shiftKey() ? WebCore::FrameSelection::Alteration::Extend
                : WebCore::FrameSelection::Alteration::Move,
            WebCore::SelectionDirection::Forward,
            platformEvent->controlKey() ? WebCore::TextGranularity::ParagraphGranularity
                : WebCore::TextGranularity::LineGranularity,
            WebCore::UserTriggered::Yes);
        break;
    case VK_HOME:
        if (platformEvent->shiftKey() && platformEvent->controlKey())
            frame->editor().command("MoveToBeginningOfDocumentAndModifySelection"_s).execute();
        else if (platformEvent->shiftKey())
            frame->editor().command("MoveToBeginningOfLineAndModifySelection"_s).execute();
        else if (platformEvent->controlKey())
            frame->editor().command("MoveToBeginningOfDocument"_s).execute();
        else
            frame->editor().command("MoveToBeginningOfLine"_s).execute();
        break;
    case VK_END:
        if (platformEvent->shiftKey() && platformEvent->controlKey())
            frame->editor().command("MoveToEndOfDocumentAndModifySelection"_s).execute();
        else if (platformEvent->shiftKey())
            frame->editor().command("MoveToEndOfLineAndModifySelection"_s).execute();
        else if (platformEvent->controlKey())
            frame->editor().command("MoveToEndOfDocument"_s).execute();
        else
            frame->editor().command("MoveToEndOfLine"_s).execute();
        break;
    case VK_PRIOR:  // PageUp
        if (platformEvent->shiftKey())
            frame->editor().command("MovePageUpAndModifySelection"_s).execute();
        else
            frame->editor().command("MovePageUp"_s).execute();
        break;
    case VK_NEXT:  // PageDown
        if (platformEvent->shiftKey())
            frame->editor().command("MovePageDownAndModifySelection"_s).execute();
        else
            frame->editor().command("MovePageDown"_s).execute();
        break;
    case VK_RETURN:
        if (platformEvent->shiftKey())
            frame->editor().command("InsertLineBreak"_s).execute();
        else
            frame->editor().command("InsertNewline"_s).execute();
        break;
    case VK_TAB:
        handled = false;
        break;
    default:
        if (!platformEvent->controlKey() && !platformEvent->altKey() && !platformEvent->text().isEmpty()) {
            if (platformEvent->text().length() == 1) {
                UChar ch = platformEvent->text()[0];
                if (ch < ' ')
                    break;
            }
            frame->editor().insertText(platformEvent->text(), event);
        } else if (platformEvent->controlKey()) {
            switch (platformEvent->windowsVirtualKeyCode()) {
            case VK_B:
                frame->editor().command("ToggleBold"_s).execute();
                break;
            case VK_I:
                frame->editor().command("ToggleItalic"_s).execute();
                break;
            case VK_A:
                frame->editor().command("SelectAll"_s).execute();
                break;
            case VK_C:
                frame->editor().command("Copy"_s).execute();
                break;
            case VK_V:
                frame->editor().command("Paste"_s).execute();
                break;
            case VK_X:
                frame->editor().command("Cut"_s).execute();
                break;
            case VK_Y:
            case VK_Z:
                if (platformEvent->shiftKey())
                    redo();
                else
                    undo();
                break;
            default:
                handled = false;
                break;
            }
        } else
            handled = false;
        break;
    }

    if (handled)
        event.setDefaultHandled();
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
