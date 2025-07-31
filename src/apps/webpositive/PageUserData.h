/*
 * Copyright 2023, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 */
#ifndef PAGE_USER_DATA_H
#define PAGE_USER_DATA_H


#include <Bitmap.h>
#include <String.h>
#include <WebView.h>


class PageUserData : public BWebView::UserData {
public:
	PageUserData(BView* focusedView)
		:
		fFocusedView(focusedView),
		fPageIcon(NULL),
		fURLInputSelectionStart(-1),
		fURLInputSelectionEnd(-1)
	{
	}

	~PageUserData()
	{
		delete fPageIcon;
	}

	void SetFocusedView(BView* focusedView)
	{
		fFocusedView = focusedView;
	}

	BView* FocusedView() const
	{
		return fFocusedView;
	}

	void SetPageIcon(const BBitmap* icon)
	{
		delete fPageIcon;
		if (icon)
			fPageIcon = new BBitmap(icon);
		else
			fPageIcon = NULL;
	}

	const BBitmap* PageIcon() const
	{
		return fPageIcon;
	}

	void SetURLInputContents(const char* text)
	{
		fURLInputContents = text;
	}

	const BString& URLInputContents() const
	{
		return fURLInputContents;
	}

	void SetURLInputSelection(int32 selectionStart, int32 selectionEnd)
	{
		fURLInputSelectionStart = selectionStart;
		fURLInputSelectionEnd = selectionEnd;
	}

	int32 URLInputSelectionStart() const
	{
		return fURLInputSelectionStart;
	}

	int32 URLInputSelectionEnd() const
	{
		return fURLInputSelectionEnd;
	}

private:
	BView*		fFocusedView;
	BBitmap*	fPageIcon;
	BString		fURLInputContents;
	int32		fURLInputSelectionStart;
	int32		fURLInputSelectionEnd;
};


#endif // PAGE_USER_DATA_H
