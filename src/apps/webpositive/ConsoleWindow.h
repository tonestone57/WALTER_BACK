/*
 * Copyright 2014-2023 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Zhuowei Zhang
 *		Humdinger
 */
#ifndef CONSOLE_WINDOW_H
#define CONSOLE_WINDOW_H


#include <String.h>
#include <Window.h>


class BListView;
class BButton;


class ConsoleWindow : public BWindow {
public:
	enum {
		MSG_ADD_MESSAGE = 'acme',
		MSG_CLEAR_MESSAGES = 'ccms',
		MSG_COPY_MESSAGES = 'cpms'
	};

public:
								ConsoleWindow(BRect frame);
	virtual						~ConsoleWindow();
	virtual	void				MessageReceived(BMessage* message);
	virtual	bool				QuitRequested();

			void				Show();

private:
			BListView*			fMessagesListView;
			BButton* 			fClearMessagesButton;
			BButton* 			fCopyMessagesButton;
			BString				fPreviousText;
			int32				fRepeatCounter;
};


#endif // CONSOLE_WINDOW_H
