/*
 * Copyright 2023, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include "FindView.h"

#include <Button.h>
#include <CheckBox.h>
#include <GroupLayoutBuilder.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>
#include <TextControl.h>
#include <Window.h>

#include "BrowserWindow.h"


static void
_InvokeButtonVisibly(BButton* button)
{
	button->SetValue(B_CONTROL_ON);
	if (button->Window())
		button->Window()->UpdateIfNeeded();
	button->Invoke();
	snooze(1000);
	button->SetValue(B_CONTROL_OFF);
}


// #pragma mark - FindView


FindView::FindView(BHandler* target)
	:
	BGroupView(B_VERTICAL, 0.0),
	fTarget(target)
{
	// Find group
	fFindCloseButton = new BButton("close button", "x", new BMessage(MSG_FIND_CLOSED));
	fFindTextControl = new BTextControl("find", "Find:", "", NULL);
	fFindTextControl->SetModificationMessage(new BMessage(MSG_FIND_TEXT_CHANGED));
	fFindPreviousButton = new BButton("Previous",
		new BMessage(MSG_FIND_PREVIOUS));
	fFindNextButton = new BButton("Next",
		new BMessage(MSG_FIND_NEXT));
	fFindCaseSensitiveCheckBox = new BCheckBox("Match case");

	SetLayout(new BGroupLayout(B_VERTICAL, 0.0));

	BView* view = BLayoutBuilder::Group<>(B_VERTICAL, 0.0)
		.Add(new BSeparatorView(B_HORIZONTAL, B_PLAIN_BORDER))
		.Add(BLayoutBuilder::Group<>(B_HORIZONTAL, B_USE_SMALL_SPACING)
			.Add(fFindCloseButton)
			.Add(fFindTextControl)
			.Add(fFindPreviousButton)
			.Add(fFindNextButton)
			.Add(fFindCaseSensitiveCheckBox)
			.SetInsets(B_USE_SMALL_INSETS)
		)
		.View();

	AddChild(view);
}


FindView::~FindView()
{
}


void
FindView::AttachedToWindow()
{
	fFindCloseButton->SetTarget(this);
	fFindPreviousButton->SetTarget(this);
	fFindNextButton->SetTarget(this);
	fFindCaseSensitiveCheckBox->SetTarget(this);
}


void
FindView::MessageReceived(BMessage* message)
{
	const char* bytes;
	int32 modifiers;
	if ((message->what == B_KEY_DOWN || message->what == B_UNMAPPED_KEY_DOWN)
		&& message->FindString("bytes", &bytes) == B_OK
		&& message->FindInt32("modifiers", &modifiers) == B_OK) {
		BView* focus = Window()->CurrentFocus();
		if (focus == fFindTextControl->TextView()) {
			if (bytes[0] == B_RETURN) {
				if ((modifiers & B_SHIFT_KEY) != 0)
					_InvokeButtonVisibly(fFindPreviousButton);
				else
					_InvokeButtonVisibly(fFindNextButton);
				return;
			} else if (bytes[0] == B_ESCAPE) {
				_InvokeButtonVisibly(fFindCloseButton);
				return;
			}
		}
	}

	switch (message->what) {
		case MSG_FIND_PREVIOUS:
		case MSG_FIND_NEXT:
		case MSG_FIND_CLOSED:
		case MSG_FIND_TEXT_CHANGED:
			fTarget.SendMessage(message);
			break;
		default:
			BGroupView::MessageReceived(message);
			break;
	}
}


void
FindView::MakeFocus(bool focused)
{
	fFindTextControl->MakeFocus(focused);
}


const char*
FindView::Text() const
{
	return fFindTextControl->Text();
}


bool
FindView::IsCaseSensitive() const
{
	return fFindCaseSensitiveCheckBox->Value() == B_CONTROL_ON;
}
