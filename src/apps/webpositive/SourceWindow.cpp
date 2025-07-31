/*
 * Copyright 2023, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 */


#include "SourceWindow.h"

#include <LayoutBuilder.h>
#include <ScrollView.h>
#include <TextView.h>


SourceWindow::SourceWindow(BRect frame, const BString& title,
	const BString& content)
	:
	BWindow(frame, title.String(), B_DOCUMENT_WINDOW, B_AUTO_UPDATE_SIZE_LIMITS)
{
	fTextView = new BTextView("source view");
	fTextView->SetText(content);
	fTextView->MakeEditable(false);

	BScrollView* scrollView = new BScrollView("source scroll view",
		fTextView, B_WILL_DRAW, true, true);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(scrollView);
}


SourceWindow::~SourceWindow()
{
}


void
SourceWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
