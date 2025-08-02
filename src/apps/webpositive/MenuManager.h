#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <MenuBar.h>

class BHandler;
class BMenu;
class BMenuItem;

class MenuManager {
public:
								MenuManager(BHandler* target);
								~MenuManager();

			BMenuBar*			MenuBar() const { return fMenuBar; }

			BMenuItem*			CutMenuItem() const { return fCutMenuItem; }
			BMenuItem*			CopyMenuItem() const { return fCopyMenuItem; }
			BMenuItem*			PasteMenuItem() const { return fPasteMenuItem; }
			BMenuItem*			FindPreviousMenuItem() const { return fFindPreviousMenuItem; }
			BMenuItem*			FindNextMenuItem() const { return fFindNextMenuItem; }
			BMenuItem*			ZoomTextOnlyMenuItem() const { return fZoomTextOnlyMenuItem; }
			BMenuItem*			FullscreenItem() const { return fFullscreenItem; }
			BMenuItem*			BackMenuItem() const { return fBackMenuItem; }
			BMenuItem*			ForwardMenuItem() const { return fForwardMenuItem; }
			BMenuItem*			BookmarkBarMenuItem() const { return fBookmarkBarMenuItem; }

			BMenu*				HistoryMenu();
			int32				HistoryMenuFixedItemCount() const { return fHistoryMenuFixedItemCount; }

private:
			BMenuBar*			fMenuBar;
			BMenu*				fHistoryMenu;
			int32				fHistoryMenuFixedItemCount;

			BMenuItem*			fCutMenuItem;
			BMenuItem*			fCopyMenuItem;
			BMenuItem*			fPasteMenuItem;
			BMenuItem*			fFindPreviousMenuItem;
			BMenuItem*			fFindNextMenuItem;
			BMenuItem*			fZoomTextOnlyMenuItem;
			BMenuItem*			fFullscreenItem;
			BMenuItem*			fBackMenuItem;
			BMenuItem*			fForwardMenuItem;
			BMenuItem*			fBookmarkBarMenuItem;
};

#endif // MENU_MANAGER_H
