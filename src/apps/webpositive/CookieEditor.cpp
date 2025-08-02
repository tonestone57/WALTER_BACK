/*
 * Copyright 2015 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adrien Destugues
 */


#include "CookieEditor.h"

#include <Button.h>
#include <Catalog.h>
#include <GroupLayoutBuilder.h>
#include <TextControl.h>


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Cookie Editor"


enum {
	COOKIE_SAVE = 'csv'
};


CookieEditor::CookieEditor(BRect frame,
	BPrivate::Network::BNetworkCookie& cookie, BWindow* parent)
	:
	BWindow(frame, B_TRANSLATE("Edit cookie"), B_TITLED_WINDOW,
		B_NORMAL_WINDOW_FEEL,
		B_AUTO_UPDATE_SIZE_LIMITS | B_ASYNCHRONOUS_CONTROLS | B_NOT_ZOOMABLE),
	fCookie(cookie)
{
	BGroupLayout* root = new BGroupLayout(B_VERTICAL, 0.0);
	SetLayout(root);

	fName = new BTextControl("name", B_TRANSLATE("Name:"), cookie.Name(), NULL);
	fValue = new BTextControl("value", B_TRANSLATE("Value:"), cookie.Value(), NULL);
	BString expiration;
	if (cookie.IsSession())
		expiration = B_TRANSLATE("Session");
	else
		expiration << cookie.ExpirationDate();
	fExpiration = new BTextControl("expiration", B_TRANSLATE("Expiration:"),
		expiration, NULL);

	BButton* saveButton = new BButton("save", B_TRANSLATE("Save"),
		new BMessage(COOKIE_SAVE));
	BButton* cancelButton = new BButton("cancel", B_TRANSLATE("Cancel"),
		new BMessage(B_QUIT_REQUESTED));

	root->AddItem(BGroupLayoutBuilder(B_VERTICAL, B_USE_DEFAULT_SPACING)
		.SetInsets(5, 5, 5, 5)
		.Add(fName)
		.Add(fValue)
		.Add(fExpiration)
		.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
			.AddGlue()
			.Add(cancelButton)
			.Add(saveButton)
		.End()
	);

	AddToSubset(parent);
}


void
CookieEditor::MessageReceived(BMessage* message)
{
	switch(message->what) {
		case COOKIE_SAVE:
		{
			fCookie.SetName(fName->Text());
			fCookie.SetValue(fValue->Text());
			struct tm time;
			if (strptime(fExpiration->Text(), "%s", &time) != NULL)
				fCookie.SetExpirationDate(mktime(&time));
			else
				fCookie.SetExpirationDate(0);
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
	}
	BWindow::MessageReceived(message);
}


bool
CookieEditor::QuitRequested()
{
	return true;
}
