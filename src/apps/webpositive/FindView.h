#ifndef FIND_VIEW_H
#define FIND_VIEW_H

#include <GroupView.h>
#include <Messenger.h>

class BButton;
class BCheckBox;
class BTextControl;

enum {
	MSG_FIND_NEXT		= 'fndn',
	MSG_FIND_PREVIOUS	= 'fndp',
	MSG_FIND_CLOSED		= 'fndc',
	MSG_FIND_TEXT_CHANGED = 'ftxt'
};

class FindView : public BGroupView {
public:
								FindView(BHandler* target);
	virtual						~FindView();

	virtual void				AttachedToWindow();
	virtual void				DispatchMessage(BMessage* message, BHandler* handler);
	virtual void				MessageReceived(BMessage* message);

			void				MakeFocus(bool focused);
			const char*			Text() const;
			bool				IsCaseSensitive() const;

private:

			BTextControl*		fFindTextControl;
			BButton*			fFindPreviousButton;
			BButton*			fFindNextButton;
			BButton*			fFindCloseButton;
			BCheckBox*			fFindCaseSensitiveCheckBox;

			BMessenger			fTarget;
};

#endif // FIND_VIEW_H
