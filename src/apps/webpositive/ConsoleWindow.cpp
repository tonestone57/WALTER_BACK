/*
 * Copyright 2014, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include "ConsoleWindow.h"

#include <Application.h>
#include <Button.h>
#include <Catalog.h>
#include <Clipboard.h>
#include <GroupLayoutBuilder.h>
#include <ListView.h>
#include <ScrollView.h>
#include <StringItem.h>


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "ConsoleWindow"

enum {
	CLEAR_CONSOLE_MESSAGES = 'ccms',
	COPY_CONSOLE_MESSAGES = 'cpms'
};


ConsoleWindow::ConsoleWindow(BRect frame)
	:
	BWindow(frame, B_TRANSLATE("Script console"), B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS | B_AUTO_UPDATE_SIZE_LIMITS)
{
	fMessagesListView = new BListView("Console messages", B_MULTIPLE_SELECTION_LIST);

	fClearMessagesButton = new BButton(B_TRANSLATE("Clear"),
		new BMessage(CLEAR_CONSOLE_MESSAGES));
	fCopyMessagesButton = new BButton(B_TRANSLATE("Copy"),
		new BMessage(COPY_CONSOLE_MESSAGES));

	SetLayout(new BGroupLayout(B_VERTICAL, 0));
	AddChild(BGroupLayoutBuilder(B_VERTICAL, 0.0)
		.Add(new BScrollView("Console messages scroll",
			fMessagesListView, 0, true, true))
		.Add(BGroupLayoutBuilder(B_HORIZONTAL, B_USE_SMALL_SPACING)
			.AddGlue()
			.Add(fClearMessagesButton)
			.Add(fCopyMessagesButton)
			.AddGlue()
		)
		.SetInsets(B_USE_DEFAULT_SPACING)
	);

	fClearMessagesButton->SetTarget(this);
	fCopyMessagesButton->SetTarget(this);
}


ConsoleWindow::~ConsoleWindow()
{
}


void
ConsoleWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case ADD_CONSOLE_MESSAGE:
		{
			BString source;
			int32 line;
			int32 column;
			BString text;

			if (message->FindString("source", &source) == B_OK
				&& message->FindInt32("line", &line) == B_OK
				&& message->FindInt32("column", &column) == B_OK
				&& message->FindString("text", &text) == B_OK) {

				text.ReplaceAll("%", "%%");
				BString finalText;
				finalText.SetToFormat("%s:%" B_PRId32 ":%" B_PRId32 ": %s",
					source.String(), line, column, text.String());
				fMessagesListView->AddItem(new BStringItem(finalText.String()));
			}
			break;
		}
		case CLEAR_CONSOLE_MESSAGES:
			for (int32 i = fMessagesListView->CountItems() - 1; i >= 0; i--)
				delete fMessagesListView->RemoveItem(i);
			break;
		case COPY_CONSOLE_MESSAGES:
		{
			BString text;
			for (int32 i = 0; i < fMessagesListView->CountItems(); i++) {
				BStringItem* item = static_cast<BStringItem*>(
					fMessagesListView->ItemAt(i));
				text << item->Text() << "\n";
			}

			if (be_clipboard->Lock()) {
				be_clipboard->Clear();
				BMessage* clip = be_clipboard->Data();
				if (clip) {
					const char* textPtr = text.String();
					ssize_t textLen = text.Length();
					clip->AddData("text/plain", B_MIME_TYPE, textPtr, textLen);
					be_clipboard->Commit();
				}
				be_clipboard->Unlock();
			}
			break;
		}
		default:
			BWindow::MessageReceived(message);
			break;
	}
}


void
ConsoleWindow::Show()
{
	if (IsHidden()) {
		BWindow::Show();
		return;
	}

	if (IsMinimized())
		SetFlags(Flags() & ~B_MINIMIZE);

	if (!IsActive())
		Activate();
}
