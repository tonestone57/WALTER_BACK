/*
 * Copyright 2023, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 */
#ifndef SOURCE_WINDOW_H
#define SOURCE_WINDOW_H


#include <Window.h>

class BTextView;


class SourceWindow : public BWindow {
public:
								SourceWindow(BRect frame, const BString& title,
									const BString& content);
	virtual						~SourceWindow();

	virtual	void				MessageReceived(BMessage* message);

private:
			BTextView*			fTextView;
};


#endif // SOURCE_WINDOW_H
