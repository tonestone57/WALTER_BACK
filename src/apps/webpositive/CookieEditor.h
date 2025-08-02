/*
 * Copyright 2015 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adrien Destugues
 */
#ifndef COOKIE_EDITOR_H
#define COOKIE_EDITOR_H


#include <Window.h>

#include <NetworkCookie.h>


class BTextControl;


class CookieEditor : public BWindow {
public:
								CookieEditor(BRect frame,
									BPrivate::Network::BNetworkCookie& cookie,
									BWindow* parent);
	virtual	void				MessageReceived(BMessage* message);
	virtual	bool				QuitRequested();

private:
			BPrivate::Network::BNetworkCookie&	fCookie;
			BTextControl*				fName;
			BTextControl*				fValue;
			BTextControl*				fExpiration;
};


#endif // COOKIE_EDITOR_H
