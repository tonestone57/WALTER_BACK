/*
 * Copyright 2023, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include "MenuManager.h"
#include "BrowserWindow.h"

#include <Application.h>
#include <Catalog.h>
#include <Directory.h>
#include <Entry.h>
#include <MenuItem.h>
#include <Path.h>
#include <PopUpMenu.h>

#include "BrowserApp.h"
#include "BrowserWindow.h"
#include "NavMenu.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "MenuManager"


class BookmarkMenu : public BNavMenu {
public:
	BookmarkMenu(const char* title, BHandler* target, const entry_ref* navDir)
		:
		BNavMenu(title, B_REFS_RECEIVED, target)
	{
		// Add these items here already, so the shortcuts work even when
		// the menu has never been opened yet.
		_AddStaticItems();

		SetNavDir(navDir);
	}

	virtual void AttachedToWindow()
	{
		RemoveItems(0, CountItems(), true);
		ForceRebuild();
		BNavMenu::AttachedToWindow();
		if (CountItems() > 0)
			AddItem(new BSeparatorItem(), 0);
		_AddStaticItems();
		DoLayout();
	}

private:
	void _AddStaticItems()
	{
		AddItem(new BMenuItem(B_TRANSLATE("Manage bookmarks"),
			new BMessage(MSG_SHOW_BOOKMARKS), 'M'), 0);
		AddItem(new BMenuItem(B_TRANSLATE("Bookmark this page"),
			new BMessage(MSG_CREATE_BOOKMARK), 'B'), 0);
	}
};


MenuManager::MenuManager(BHandler* target)
	:
	fHistoryMenu(NULL)
{
	fMenuBar = new BMenuBar("Main menu");
	BMenu* menu = new BMenu(B_TRANSLATE("Window"));
	BMessage* newWindowMessage = new BMessage(NEW_WINDOW);
	newWindowMessage->AddString("url", "");
	BMenuItem* newItem = new BMenuItem(B_TRANSLATE("New window"),
		newWindowMessage, 'N');
	menu->AddItem(newItem);
	newItem->SetTarget(be_app);
	BMessage newTabMessage(NEW_TAB);
	newTabMessage.AddString("url", "");
	newTabMessage.AddPointer("window", target);
	newTabMessage.AddBool("select", true);
	newItem = new BMenuItem(B_TRANSLATE("New tab"),
		new BMessage(newTabMessage), 'T');
	menu->AddItem(newItem);
	newItem->SetTarget(be_app);
	menu->AddItem(new BMenuItem(B_TRANSLATE("Open location"),
		new BMessage(MSG_OPEN_LOCATION), 'L'));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem(B_TRANSLATE("Close window"),
		new BMessage(B_QUIT_REQUESTED), 'W', B_SHIFT_KEY));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Close tab"),
		new BMessage(MSG_CLOSE_TAB), 'W'));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Save page as" B_UTF8_ELLIPSIS),
		new BMessage(MSG_SAVE_PAGE), 'S'));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem(B_TRANSLATE("Downloads"),
		new BMessage(SHOW_DOWNLOAD_WINDOW), 'D'));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Settings"),
		new BMessage(SHOW_SETTINGS_WINDOW), ','));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Cookie manager"),
		new BMessage(SHOW_COOKIE_WINDOW)));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Script console"),
		new BMessage(SHOW_CONSOLE_WINDOW)));
	BMenuItem* aboutItem = new BMenuItem(B_TRANSLATE("About"),
		new BMessage(B_ABOUT_REQUESTED));
	menu->AddItem(aboutItem);
	aboutItem->SetTarget(be_app);
	menu->AddSeparatorItem();
	BMenuItem* quitItem = new BMenuItem(B_TRANSLATE("Quit"),
		new BMessage(B_QUIT_REQUESTED), 'Q');
	menu->AddItem(quitItem);
	quitItem->SetTarget(be_app);
	fMenuBar->AddItem(menu);

	menu = new BMenu(B_TRANSLATE("Edit"));
	menu->AddItem(fCutMenuItem = new BMenuItem(B_TRANSLATE("Cut"),
		new BMessage(B_CUT), 'X'));
	menu->AddItem(fCopyMenuItem = new BMenuItem(B_TRANSLATE("Copy"),
		new BMessage(B_COPY), 'C'));
	menu->AddItem(fPasteMenuItem = new BMenuItem(B_TRANSLATE("Paste"),
		new BMessage(B_PASTE), 'V'));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem(B_TRANSLATE("Find"),
		new BMessage(MSG_SHOW_FIND_GROUP), 'F'));
	menu->AddItem(fFindPreviousMenuItem
		= new BMenuItem(B_TRANSLATE("Find previous"),
		new BMessage(MSG_FIND_PREVIOUS), 'G', B_SHIFT_KEY));
	menu->AddItem(fFindNextMenuItem = new BMenuItem(B_TRANSLATE("Find next"),
		new BMessage(MSG_FIND_NEXT), 'G'));
	fMenuBar->AddItem(menu);
	fFindPreviousMenuItem->SetEnabled(false);
	fFindNextMenuItem->SetEnabled(false);

	menu = new BMenu(B_TRANSLATE("View"));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Reload"), new BMessage(MSG_RELOAD),
		'R'));
	// the label will be replaced with the appropriate text later on
	fBookmarkBarMenuItem = new BMenuItem(B_TRANSLATE("Show bookmark bar"),
		new BMessage(MSG_SHOW_HIDE_BOOKMARK_BAR));
	menu->AddItem(fBookmarkBarMenuItem);
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem(B_TRANSLATE("Increase size"),
		new BMessage(MSG_ZOOM_FACTOR_INCREASE), '+'));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Decrease size"),
		new BMessage(MSG_ZOOM_FACTOR_DECREASE), '-'));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Reset size"),
		new BMessage(MSG_ZOOM_FACTOR_RESET), '0'));
	fZoomTextOnlyMenuItem = new BMenuItem(B_TRANSLATE("Zoom text only"),
		new BMessage(MSG_ZOOM_TEXT_ONLY));
	menu->AddItem(fZoomTextOnlyMenuItem);

	menu->AddSeparatorItem();
	fFullscreenItem = new BMenuItem(B_TRANSLATE("Full screen"),
		new BMessage(MSG_TOGGLE_FULLSCREEN), B_RETURN);
	menu->AddItem(fFullscreenItem);
	menu->AddItem(new BMenuItem(B_TRANSLATE("Page source"),
		new BMessage(MSG_SHOW_PAGE_SOURCE), 'U'));
	fMenuBar->AddItem(menu);

	fHistoryMenu = new BMenu(B_TRANSLATE("History"));
	fHistoryMenu->AddItem(fBackMenuItem = new BMenuItem(B_TRANSLATE("Back"),
		new BMessage(MSG_GO_BACK), B_LEFT_ARROW));
	fHistoryMenu->AddItem(fForwardMenuItem
		= new BMenuItem(B_TRANSLATE("Forward"), new BMessage(MSG_GO_FORWARD),
		B_RIGHT_ARROW));
	fHistoryMenu->AddSeparatorItem();
	fHistoryMenuFixedItemCount = fHistoryMenu->CountItems();
	fMenuBar->AddItem(fHistoryMenu);

	BPath bookmarkPath;
	entry_ref bookmarkRef;
	if (static_cast<BrowserWindow*>(target)->BookmarkPath(bookmarkPath) == B_OK
		&& get_ref_for_path(bookmarkPath.Path(), &bookmarkRef) == B_OK) {
		BMenu* bookmarkMenu
			= new BookmarkMenu(B_TRANSLATE("Bookmarks"), target, &bookmarkRef);
		fMenuBar->AddItem(bookmarkMenu);

		BDirectory barDir(&bookmarkRef);
		BEntry bookmarkBar(&barDir, "Bookmark bar");
		entry_ref bookmarkBarRef;
		if (bookmarkBar.Exists() && bookmarkBar.GetRef(&bookmarkBarRef) == B_OK) {
			BDirectory dir(&bookmarkBarRef);
			if (dir.CountEntries() > 0)
				fBookmarkBarMenuItem->SetEnabled(true);
			else
				fBookmarkBarMenuItem->SetEnabled(false);
		} else
			fBookmarkBarMenuItem->SetEnabled(false);
	} else
		fBookmarkBarMenuItem->SetEnabled(false);
}


MenuManager::~MenuManager()
{
	delete fMenuBar;
}


BMenu*
MenuManager::HistoryMenu()
{
    if (fHistoryMenu != nullptr)
        return fHistoryMenu;

    // Create the History menu
    fHistoryMenu = new BMenu(B_TRANSLATE("History"));

    // Add Back and Forward items with keyboard shortcuts
    fBackMenuItem = new BMenuItem(B_TRANSLATE("Back"), new BMessage(MSG_GO_BACK), B_LEFT_ARROW);
    fForwardMenuItem = new BMenuItem(B_TRANSLATE("Forward"), new BMessage(MSG_GO_FORWARD), B_RIGHT_ARROW);

    fHistoryMenu->AddItem(fBackMenuItem);
    fHistoryMenu->AddItem(fForwardMenuItem);
    fHistoryMenu->AddSeparatorItem();

    // Store the count of fixed items
    fHistoryMenuFixedItemCount = fHistoryMenu->CountItems();

    // Add the History menu to the menu bar
    fMenuBar->AddItem(fHistoryMenu);

    return fHistoryMenu;
}
