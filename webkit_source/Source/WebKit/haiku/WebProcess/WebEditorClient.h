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
#ifndef WebEditorClient_h
#define WebEditorClient_h

#include <WebCore/EditorClient.h>
#include <WebCore/TextChecking.h>
#include <wtf/Forward.h>

namespace WebKit {

class WebPage;

class WebEditorClient final : public WebCore::EditorClient {
public:
    WebEditorClient(WebPage* page);
    ~WebEditorClient() = default;

private:
    bool shouldDeleteRange(const std::optional<WebCore::SimpleRange>&) override;
    bool smartInsertDeleteEnabled() override;
    bool isSelectTrailingWhitespaceEnabled() const override;
    bool isContinuousSpellCheckingEnabled() override;
    void toggleContinuousSpellChecking() override;
    bool isGrammarCheckingEnabled() override;
    void toggleGrammarChecking() override;
    int spellCheckerDocumentTag() override;

    bool shouldBeginEditing(const WebCore::SimpleRange&) override;
    bool shouldEndEditing(const WebCore::SimpleRange&) override;
    bool shouldInsertNode(WebCore::Node&, const std::optional<WebCore::SimpleRange>&, WebCore::EditorInsertAction) override;
    bool shouldInsertText(const String&, const std::optional<WebCore::SimpleRange>&, WebCore::EditorInsertAction) override;
    bool shouldChangeSelectedRange(const std::optional<WebCore::SimpleRange>& fromRange, const std::optional<WebCore::SimpleRange>& toRange, WebCore::Affinity, bool stillSelecting) override;

    bool shouldApplyStyle(const WebCore::StyleProperties&, const std::optional<WebCore::SimpleRange>&) override;
    void didApplyStyle() override;
    bool shouldMoveRangeAfterDelete(const WebCore::SimpleRange&, const WebCore::SimpleRange&) override;

    void didBeginEditing() override;
    void respondToChangedContents() override;
    void respondToChangedSelection(WebCore::LocalFrame*) override;
    void didEndUserTriggeredSelectionChanges() override;
    void updateEditorStateAfterLayoutIfEditabilityChanged() override;
    void didEndEditing() override;
    void willWriteSelectionToPasteboard(const std::optional<WebCore::SimpleRange>&) override;
    void didWriteSelectionToPasteboard() override;
    void getClientPasteboardData(const std::optional<WebCore::SimpleRange>&, Vector<std::pair<String, RefPtr<WebCore::SharedBuffer>>>&) override;

    WebCore::DOMPasteAccessResponse requestDOMPasteAccess(WebCore::DOMPasteAccessCategory, WebCore::FrameIdentifier, const String& originIdentifier) override;

    void discardedComposition(const WebCore::Document&) override;
    void canceledComposition() override;
    void didUpdateComposition() override;

    void registerUndoStep(WebCore::UndoStep&) override;
    void registerRedoStep(WebCore::UndoStep&) override;
    void clearUndoRedoOperations() override;

    bool canCopyCut(WebCore::LocalFrame*, bool defaultValue) const override;
    bool canPaste(WebCore::LocalFrame*, bool defaultValue) const override;
    bool canUndo() const override;
    bool canRedo() const override;

    void undo() override;
    void redo() override;

    void handleKeyboardEvent(WebCore::KeyboardEvent&) override;
    void handleInputMethodKeydown(WebCore::KeyboardEvent&) override;

    void textFieldDidBeginEditing(WebCore::Element&) override;
    void textFieldDidEndEditing(WebCore::Element&) override;
    void textDidChangeInTextField(WebCore::Element&) override;
    bool doTextFieldCommandFromEvent(WebCore::Element&, WebCore::KeyboardEvent*) override;
    void textWillBeDeletedInTextField(WebCore::Element&) override;
    void textDidChangeInTextArea(WebCore::Element&) override;
    void overflowScrollPositionChanged() override;
    void subFrameScrollPositionChanged() override;

#if PLATFORM(COCOA)
    void setInsertionPasteboard(const String& pasteboardName) override;
#endif

#if USE(APPKIT)
    void uppercaseWord() override;
    void lowercaseWord() override;
    void capitalizeWord() override;
#endif

#if USE(AUTOMATIC_TEXT_REPLACEMENT)
    void showSubstitutionsPanel(bool show) override;
    bool substitutionsPanelIsShowing() override;
    void toggleSmartInsertDelete() override;
    bool isAutomaticQuoteSubstitutionEnabled() override;
    void toggleAutomaticQuoteSubstitution() override;
    bool isAutomaticLinkDetectionEnabled() override;
    void toggleAutomaticLinkDetection() override;
    bool isAutomaticDashSubstitutionEnabled() override;
    void toggleAutomaticDashSubstitution() override;
    bool isAutomaticTextReplacementEnabled() override;
    void toggleAutomaticTextReplacement() override;
    bool isAutomaticSpellingCorrectionEnabled() override;
    void toggleAutomaticSpellingCorrection() override;
#endif

    WebCore::TextCheckerClient* textChecker() override;

    void updateSpellingUIWithGrammarString(const String&, const WebCore::GrammarDetail& detail) override;
    void updateSpellingUIWithMisspelledWord(const String&) override;
    void showSpellingUI(bool show) override;
    bool spellingUIIsShowing() override;
    void setInputMethodState(WebCore::Element*) override;

    bool performTwoStepDrop(WebCore::DocumentFragment&, const WebCore::SimpleRange& destination, bool isMove) override;

    WebPage* m_page;
};

} // namespace WebKit

#endif // WebEditorClient_h
