/*
 * Copyright (C) 2007 Andrea Anzani <andrea.anzani@gmail.com>
 * Copyright (C) 2007, 2010 Ryan Leavengood <leavengood@gmail.com>
 * Copyright (C) 2009 Maxime Simon <simon.maxime@gmail.com>
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 * Copyright (C) 2010 Michael Lotz <mmlr@mlotz.ch>
 * Copyright (C) 2010 Rene Gollent <rene@gollent.com>
 * Copyright 2013-2015 Haiku, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "BrowserWindow.h"

#include "DataLoader.h"

#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <Button.h>
#include <Catalog.h>
#include <CheckBox.h>
#include <Clipboard.h>
#include <ControlLook.h>
#include <Debug.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FilePanel.h>
#include <FindDirectory.h>
#include <GridLayoutBuilder.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <IconMenuItem.h>
#include <Keymap.h>
#include <LayoutBuilder.h>
#include <Locale.h>
#include <ObjectList.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <MessageRunner.h>
#include <NodeInfo.h>
#include <NodeMonitor.h>
#include <Path.h>
#include <Roster.h>
#include <Screen.h>
#include <SeparatorView.h>
#include <Size.h>
#include <SpaceLayoutItem.h>
#include <StatusBar.h>
#include <StringView.h>
#include <TextControl.h>
#include <UnicodeChar.h>
#include <Url.h>

#include <map>
#include <cstdio>

#include "AuthenticationPanel.h"
#include "BaseURL.h"
#include "BitmapButton.h"
#include "BookmarkBar.h"
#include "BookmarkManager.h"
#include "BrowserApp.h"
#include "BrowsingHistory.h"
#include "CredentialsStorage.h"
#include "FindView.h"
#include "IconButton.h"
#include "MenuManager.h"
#include "NavMenu.h"
#include "SettingsKeys.h"
#include "SettingsMessage.h"
#include "TabManager.h"
#include "URLInputGroup.h"
#include "WebPage.h"
#include "WebView.h"
#include "WebViewConstants.h"
#include "WindowIcon.h"
#include "SourceWindow.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "WebPositive Window"


enum {
	MSG_POPULATE_HISTORY_MENU					= 'phmn',
	MSG_DATA_LOADED								= 'dtld',
	OPEN_LOCATION								= 'open',
	SAVE_PAGE									= 'save',
	GO_BACK										= 'goba',
	GO_FORWARD									= 'gofo',
	STOP										= 'stop',
	HOME										= 'home',
	GOTO_URL									= 'goul',
	RELOAD										= 'reld',
	SHOW_HIDE_BOOKMARK_BAR						= 'shbb',
	CLEAR_HISTORY								= 'clhs',

	CREATE_BOOKMARK								= 'crbm',
	SHOW_BOOKMARKS								= 'shbm',

	ZOOM_FACTOR_INCREASE						= 'zfin',
	ZOOM_FACTOR_DECREASE						= 'zfdc',
	ZOOM_FACTOR_RESET							= 'zfrs',
	ZOOM_TEXT_ONLY								= 'zfto',

	TOGGLE_FULLSCREEN							= 'tgfs',
	TOGGLE_AUTO_HIDE_INTERFACE_IN_FULLSCREEN	= 'tgah',
	CHECK_AUTO_HIDE_INTERFACE					= 'cahi',

	SHOW_PAGE_SOURCE							= 'spgs',
	SHOW_HISTORY_WINDOW							= 'shhw',

	EDIT_SHOW_FIND_GROUP						= 'sfnd',
	EDIT_HIDE_FIND_GROUP						= 'hfnd',
	EDIT_FIND_NEXT								= 'fndn',
	EDIT_FIND_PREVIOUS							= 'fndp',

	SELECT_TAB									= 'sltb',
	CYCLE_TABS									= 'ctab',
};


static const int32 kModifiers = B_SHIFT_KEY | B_COMMAND_KEY
	| B_CONTROL_KEY | B_OPTION_KEY | B_MENU_KEY;


static const char* kHandledProtocols[] = {
	"http",
	"https",
	"ftp",
	"file",
	"about",
	"data",
	"gopher"
};

static const char* kBookmarkBarSubdir = "Bookmark bar";

static BLayoutItem*
layoutItemFor(BView* view)
{
	BLayout* layout = view->Parent()->GetLayout();
	int32 index = layout->IndexOfView(view);
	return layout->ItemAt(index);
}


class CloseButton : public BButton {
public:
	CloseButton(BMessage* message)
		:
		BButton("close button", NULL, message),
		fOverCloseRect(false)
	{
		// Button is 16x16 regardless of font size
		SetExplicitMinSize(BSize(15, 15));
		SetExplicitMaxSize(BSize(15, 15));
	}

	virtual void Draw(BRect updateRect)
	{
		BRect frame = Bounds();
		BRect closeRect(frame.InsetByCopy(4, 4));
		rgb_color base = ui_color(B_PANEL_BACKGROUND_COLOR);
		float tint = B_DARKEN_1_TINT;

		if (fOverCloseRect)
			tint *= 1.4;
		else
			tint *= 1.2;

		uint32 flags = 0;
		if (Value() == B_CONTROL_ON && fOverCloseRect)
			flags = BControlLook::B_ACTIVATED;

		be_control_look->DrawButtonFrame(this, frame, updateRect,
				base, base, tint, flags);
		be_control_look->DrawButtonBackground(this, frame, updateRect,
				base, flags);

		if (Value() == B_CONTROL_ON && fOverCloseRect) {
			closeRect.OffsetBy(1, 1);
			tint *= 1.2;
		}

		// Draw the ×
		base = tint_color(base, tint);
		SetHighColor(base);
		SetPenSize(2);
		StrokeLine(closeRect.LeftTop(), closeRect.RightBottom());
		StrokeLine(closeRect.LeftBottom(), closeRect.RightTop());
		SetPenSize(1);
	}

	virtual void MouseMoved(BPoint where, uint32 transit,
		const BMessage* dragMessage)
	{
		switch (transit) {
			case B_ENTERED_VIEW:
				fOverCloseRect = true;
				Invalidate();
				break;
			case B_EXITED_VIEW:
				fOverCloseRect = false;
				Invalidate();
				break;
			case B_INSIDE_VIEW:
				fOverCloseRect = true;
				break;
			case B_OUTSIDE_VIEW:
				fOverCloseRect = false;
				break;
		}

		BButton::MouseMoved(where, transit, dragMessage);
	}

private:
	bool fOverCloseRect;
};


// #pragma mark - BrowserWindow


BrowserWindow::BrowserWindow(BRect frame, SettingsMessage* appSettings, const BString& url,
	BPrivate::Network::BUrlContext* context, uint32 interfaceElements, BWebView* webView,
	uint32 workspaces)
	:
	BWebWindow(frame, kApplicationName, B_DOCUMENT_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
		B_AUTO_UPDATE_SIZE_LIMITS | B_ASYNCHRONOUS_CONTROLS, workspaces),
	fIsFullscreen(false),
	fInterfaceVisible(false),
	fMenusRunning(false),
	fPulseRunner(),
	fVisibleInterfaceElements(interfaceElements),
	fHistoryItems(NULL),
	fMenuManager(std::make_unique<MenuManager>(this)),
	fLastClipboardCheckTime(0),
	fContext(context),
	fAppSettings(appSettings),
	fZoomTextOnly(false),
	fShowTabsIfSinglePageOpen(true),
	fAutoHideInterfaceInFullscreenMode(false),
	fAutoHidePointer(false),
	fBookmarkBar(),
	fBookmarkManager(std::make_unique<BookmarkManager>()),
	fDataLoader(std::make_unique<DataLoader>(BMessenger(this)))
{
	// Begin listening to settings changes and read some current values.
	fAppSettings->AddListener(BMessenger(this));
	fDataLoader->Start();
	fZoomTextOnly = fAppSettings->GetValue("zoom text only", fZoomTextOnly);
	fShowTabsIfSinglePageOpen = fAppSettings->GetValue(
		kSettingsKeyShowTabsIfSinglePageOpen, fShowTabsIfSinglePageOpen);

	fAutoHidePointer = fAppSettings->GetValue(kSettingsKeyAutoHidePointer,
		fAutoHidePointer);

	fNewWindowPolicy = fAppSettings->GetValue(kSettingsKeyNewWindowPolicy,
		(uint32)OpenStartPage);
	fNewTabPolicy = fAppSettings->GetValue(kSettingsKeyNewTabPolicy,
		(uint32)OpenBlankPage);
	fStartPageURL = fAppSettings->GetValue(kSettingsKeyStartPageURL,
		kDefaultStartPageURL);
	fSearchPageURL = fAppSettings->GetValue(kSettingsKeySearchPageURL,
		kDefaultSearchPageURL);

	// Create the interface elements
	BMessage* newTabMessage = new BMessage(NEW_TAB);
	newTabMessage->AddString("url", "");
	newTabMessage->AddPointer("window", this);
	newTabMessage->AddBool("select", true);
	fTabManager = std::make_unique<TabManager>(BMessenger(this), newTabMessage);


	// Back, Forward, Stop & Home buttons
	fBackButton = new BIconButton("Back", NULL, new BMessage(GO_BACK));
	fBackButton->SetIcon(201);
	fBackButton->TrimIcon();

	fForwardButton = new BIconButton("Forward", NULL, new BMessage(GO_FORWARD));
	fForwardButton->SetIcon(202);
	fForwardButton->TrimIcon();

	fStopButton = new BIconButton("Stop", NULL, new BMessage(STOP));
	fStopButton->SetIcon(204);
	fStopButton->TrimIcon();

	fHomeButton = new BIconButton("Home", NULL, new BMessage(HOME));
	fHomeButton->SetIcon(206);
	fHomeButton->TrimIcon();
	if (!fAppSettings->GetValue(kSettingsKeyShowHomeButton, true))
		fHomeButton->Hide();

	// URL input group
	fURLInputGroup = new URLInputGroup(new BMessage(GOTO_URL));

	// Status Bar
	fStatusText = new BStringView("status", "");
	fStatusText->SetAlignment(B_ALIGN_LEFT);
	fStatusText->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
	fStatusText->SetExplicitMinSize(BSize(150, 12));
		// Prevent the window from growing to fit a long status message...
	BFont font(be_plain_font);
	font.SetSize(ceilf(font.Size() * 0.8));
	fStatusText->SetFont(&font, B_FONT_SIZE);

	// Loading progress bar
	fLoadingProgressBar = new BStatusBar("progress");
	fLoadingProgressBar->SetMaxValue(100);
	fLoadingProgressBar->Hide();
	font_height height;
	font.GetHeight(&height);
	fLoadingProgressBar->SetBarHeight(height.ascent + height.descent);

	const float kInsetSpacing = 3;
	const float kElementSpacing = 5;

	fFindView = new FindView(this);

	// Navigation group
	BGroupLayout* navigationGroup = BLayoutBuilder::Group<>(B_VERTICAL, 0.0)
		.Add(BLayoutBuilder::Group<>(B_HORIZONTAL, kElementSpacing)
			.Add(fBackButton)
			.Add(fForwardButton)
			.Add(fStopButton)
			.Add(fHomeButton)
			.Add(fURLInputGroup)
			.SetInsets(kInsetSpacing, kInsetSpacing, kInsetSpacing,
				kInsetSpacing)
		)
		.Add(new BSeparatorView(B_HORIZONTAL, B_PLAIN_BORDER))
	;

	// Status bar group
	BGroupLayout* statusGroup = BLayoutBuilder::Group<>(B_VERTICAL, 0.0)
		.Add(new BSeparatorView(B_HORIZONTAL, B_PLAIN_BORDER))
		.Add(BLayoutBuilder::Group<>(B_HORIZONTAL, kElementSpacing)
			.Add(fStatusText)
			.Add(fLoadingProgressBar, 0.2)
			.AddStrut(12 - kElementSpacing)
			.SetInsets(kInsetSpacing, 0, kInsetSpacing, 0)
		)
	;


	BBitmapButton* toggleFullscreenButton = new BBitmapButton(kWindowIconBits,
		kWindowIconWidth, kWindowIconHeight, kWindowIconFormat,
		new BMessage(TOGGLE_FULLSCREEN));
	toggleFullscreenButton->SetBackgroundMode(BBitmapButton::MENUBAR_BACKGROUND);

	fMenuGroup = (new BGroupView(B_HORIZONTAL, 0))->GroupLayout();
	BLayoutBuilder::Group<>(fMenuGroup)
		.Add(fMenuManager->MenuBar())
		.Add(toggleFullscreenButton, 0.0f)
	;

	if (fAppSettings->GetValue(kSettingsShowBookmarkBar, true))
		_ShowBookmarkBar(true);
	else
		_ShowBookmarkBar(false);

	BPath bookmarkPath;
	entry_ref bookmarkRef;
	if (_BookmarkPath(bookmarkPath) == B_OK
		&& get_ref_for_path(bookmarkPath.Path(), &bookmarkRef) == B_OK) {
		BDirectory barDir(&bookmarkRef);
		BEntry bookmarkBar(&barDir, kBookmarkBarSubdir);
		entry_ref bookmarkBarRef;
		if (bookmarkBar.Exists() && bookmarkBar.GetRef(&bookmarkBarRef) == B_OK) {
			BDirectory dir(&bookmarkBarRef);
			if (dir.CountEntries() > 0) {
				fBookmarkBar.reset(new BookmarkBar("Bookmarks", this, &bookmarkBarRef));
			}
		}
	}

	fSavePanel = std::make_unique<BFilePanel>(B_SAVE_PANEL, new BMessenger(this), nullptr, 0, false);

	// Layout
	BGroupView* topView = new BGroupView(B_VERTICAL, 0.0);
	AddChild(topView);

	BGroupView* container = new BGroupView(B_VERTICAL, 0.0);
	container->SetName("interface_container");
	topView->AddChild(container);

#if !INTEGRATE_MENU_INTO_TAB_BAR
	container->AddChild(fMenuGroup);
#endif
	container->AddChild(fTabManager->TabGroup());
	container->AddChild(navigationGroup);
	if (fBookmarkBar)
		container->AddChild(fBookmarkBar.get());
	topView->AddChild(fTabManager->ContainerView());
	container->AddChild(fFindView);
	container->AddChild(statusGroup);

	fURLInputGroup->MakeFocus(true);

	fTabGroup = fTabManager->TabGroup()->GetLayout();
	fNavigationGroup = navigationGroup;
	fStatusGroup = statusGroup;
	fToggleFullscreenButton = layoutItemFor(toggleFullscreenButton);

	fFindView->SetVisible(false);
	fToggleFullscreenButton->SetVisible(false);

	CreateNewTab(url, true, webView);
	_ShowInterface(true);
	_SetAutoHideInterfaceInFullscreen(fAppSettings->GetValue(
		kSettingsKeyAutoHideInterfaceInFullscreenMode,
		fAutoHideInterfaceInFullscreenMode));

	AddShortcut('F', B_COMMAND_KEY | B_SHIFT_KEY,
		new BMessage(EDIT_HIDE_FIND_GROUP));
	// TODO: Should be a different shortcut, H is usually for Find selection.
	AddShortcut('H', B_COMMAND_KEY | B_SHIFT_KEY, new BMessage(HOME));

	// Add shortcuts to select a particular tab
	for (int32 i = 1; i <= 9; i++) {
		BMessage* selectTab = new BMessage(SELECT_TAB);
		selectTab->AddInt32("tab index", i - 1);
		char numStr[2];
		snprintf(numStr, sizeof(numStr), "%d", (int) i);
		AddShortcut(numStr[0], B_COMMAND_KEY, selectTab);
	}

	// Add shortcut to cycle through tabs like in every other web browser
	AddShortcut(B_TAB, B_COMMAND_KEY, new BMessage(CYCLE_TABS));

	BKeymap keymap;
	keymap.SetToCurrent();
	BStringList unmodified(3);
	if (keymap.GetModifiedCharacters("+", B_SHIFT_KEY, 0, unmodified)
			== B_OK) {
		int32 count = unmodified.CountStrings();
		for (int32 i = 0; i < count; i++) {
			uint32 key = BUnicodeChar::FromUTF8(unmodified.StringAt(i));
			if (!HasShortcut(key, 0)) {
				// Add semantic zoom in shortcut, bug #7428
				AddShortcut(key, B_COMMAND_KEY,
					new BMessage(ZOOM_FACTOR_INCREASE));
			}
		}
	}
	unmodified.MakeEmpty();

	be_app->PostMessage(WINDOW_OPENED);
}


BrowserWindow::~BrowserWindow()
{
	fAppSettings->RemoveListener(BMessenger(this));
	delete fHistoryItems;
}


void
BrowserWindow::DispatchMessage(BMessage* message, BHandler* target)
{
	const char* bytes;
	int32 modifierKeys;
	if ((message->what == B_KEY_DOWN || message->what == B_UNMAPPED_KEY_DOWN)
		&& message->FindString("bytes", &bytes) == B_OK
		&& message->FindInt32("modifiers", &modifierKeys) == B_OK) {
		if (bytes[0] == B_FUNCTION_KEY) {
			// Some function key Firefox compatibility
			int32 key;
			if (message->FindInt32("key", &key) == B_OK) {
				switch (key) {
					case B_F5_KEY:
						PostMessage(RELOAD);
						break;

					case B_F11_KEY:
						PostMessage(TOGGLE_FULLSCREEN);
						break;

					default:
						break;
				}
			}
		} else if (target == fURLInputGroup->TextView()) {
			// Handle B_RETURN in the URL text control. This is the easiest
			// way to react *only* when the user presses the return key in the
			// address bar, as opposed to trying to load whatever is in there
			// when the text control just goes out of focus.
			if (bytes[0] == B_RETURN) {
				// Do it in such a way that the user sees the Go-button go down.
				_InvokeButtonVisibly(fURLInputGroup->GoButton());
			} else if (bytes[0] == B_ESCAPE) {
				// Replace edited text with the current URL.
				fURLInputGroup->LockURLInput(false);
				fURLInputGroup->SetText(BString(CurrentWebView()->MainFrameURL()));
			}
		} else if (bytes[0] == B_ESCAPE && !fMenusRunning) {
			if (modifierKeys == B_COMMAND_KEY)
				_ShowInterface(true);
			else {
				// Default escape key behavior:
				PostMessage(STOP);
				return;
			}
		}
	}

	if (message->what == B_MOUSE_MOVED || message->what == B_MOUSE_DOWN
		|| message->what == B_MOUSE_UP) {
		message->FindPoint("where", &fLastMousePos);
		if (message->FindInt64("when", &fLastMouseMovedTime) != B_OK)
			fLastMouseMovedTime = system_time();
		_CheckAutoHideInterface();
	}

	if (message->what == B_MOUSE_WHEEL_CHANGED) {
		BPoint where;
		uint32 buttons;
		CurrentWebView()->GetMouse(&where, &buttons, false);
		// Only do this when the mouse is over the web view
		if (CurrentWebView()->Bounds().Contains(where)) {
			// Zoom and unzoom text on Command + mouse wheel.
			// This could of course (and maybe should be) implemented in the
			// WebView, but there would need to be a way for the WebView to
			// know the setting of the fZoomTextOnly member here. Plus other
			// clients of the API may not want this feature.
			if ((modifiers() & B_COMMAND_KEY) != 0) {
				float deltaY;
				if (message->FindFloat("be:wheel_delta_y", &deltaY) == B_OK) {
					if (deltaY < 0)
						CurrentWebView()->IncreaseZoomFactor(fZoomTextOnly);
					else
						CurrentWebView()->DecreaseZoomFactor(fZoomTextOnly);

					return;
				}
			}
		} else {
			// Also don't scroll up and down if the mouse is not over the
			// web view
			return;
		}
	}

	BWebWindow::DispatchMessage(message, target);
}


void
BrowserWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case OPEN_LOCATION:
			_ShowInterface(true);
			if (fURLInputGroup->TextView()->IsFocus())
				fURLInputGroup->TextView()->SelectAll();
			else
				fURLInputGroup->MakeFocus(true);
			break;

		case RELOAD:
			CurrentWebView()->Reload();
			break;

		case SHOW_HIDE_BOOKMARK_BAR:
			_ShowBookmarkBar(fBookmarkBar->IsHidden());
			break;

		case GOTO_URL:
		{
			BString url;
			if (message->FindString("url", &url) != B_OK)
				url = fURLInputGroup->Text();

			_SetPageIcon(CurrentWebView(), NULL);
			_SmartURLHandler(url);

			break;
		}

		case SAVE_PAGE:
		{
			fSavePanel->SetSaveText(CurrentWebView()->MainFrameTitle());
			fSavePanel->Show();
			break;
		}

		case B_SAVE_REQUESTED:
		{
			entry_ref ref;
			BString name;

			if (message->FindRef("directory", &ref) == B_OK
				&& message->FindString("name", &name) == B_OK) {
				BDirectory dir(&ref);
				BFile output(&dir, name,
					B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
				CurrentWebView()->WebPage()->GetContentsAsMHTML(output);
			}

			break;
		}

		case GO_BACK:
			CurrentWebView()->GoBack();
			break;

		case GO_FORWARD:
			CurrentWebView()->GoForward();
			break;

		case STOP:
			CurrentWebView()->StopLoading();
			break;

		case HOME:
			CurrentWebView()->LoadURL(fStartPageURL);
			break;

		case CLEAR_HISTORY: {
			BrowsingHistory* history = BrowsingHistory::DefaultInstance();
			if (history->CountItems() == 0)
				break;
			BAlert* alert = new BAlert(B_TRANSLATE("Confirmation"),
				B_TRANSLATE("Do you really want to "
				"clear the browsing history?"), B_TRANSLATE("Clear"),
				B_TRANSLATE("Cancel"));
			alert->SetShortcut(1, B_ESCAPE);

			if (alert->Go() == 0)
				history->Clear();
			break;
		}

		case CREATE_BOOKMARK:
			fBookmarkManager->CreateBookmark(this);
			break;

		case SHOW_BOOKMARKS:
			fBookmarkManager->ShowBookmarks();
			break;

		case B_REFS_RECEIVED:
		{
			// Currently the only source of these messages is the bookmarks
			// menu.
			// Filter refs into URLs, this also gets rid of refs for folders.
			// For clicks on sub-folders in the bookmarks menu, we have Tracker
			// open the corresponding folder.
			entry_ref ref;
			uint32 addedCount = 0;
			for (int32 i = 0; message->FindRef("refs", i, &ref) == B_OK; i++) {
				BEntry entry(&ref);
				uint32 addedSubCount = 0;
				if (entry.IsDirectory()) {
					BDirectory directory(&entry);
					fBookmarkManager->_AddBookmarkURLsRecursively(directory,
						message, addedSubCount);
				} else {
					BFile file(&ref, B_READ_ONLY);
					BString url;
					if (fBookmarkManager->_ReadURLAttr(file, url)) {
						message->AddString("url", url.String());
						addedSubCount++;
					}
				}
				if (addedSubCount == 0) {
					// Don't know what to do with this entry, just pass it
					// on to the system to handle. Note that this may result
					// in us opening other supported files via the application
					// mechanism.
					if (be_roster->Launch(&ref) != B_OK) {
						BAlert* alert = new BAlert(B_TRANSLATE("Open bookmark error"),
							B_TRANSLATE("The bookmark could not be opened."),
							B_TRANSLATE("OK"));
						alert->SetFlags(alert->Flags() | B_CLOSE_ON_ESCAPE);
						alert->Go(NULL);
					}
				}
				addedCount += addedSubCount;
			}
			message->RemoveName("refs");
			if (addedCount > 10) {
				BString string(B_TRANSLATE_COMMENT("Do you want to open "
					"%addedCount bookmarks all at once?", "Don't translate "
					"variable %addedCount."));
				string.ReplaceFirst("%addedCount", BString() << addedCount);

				BAlert* alert = new BAlert(
					B_TRANSLATE("Open bookmarks confirmation"),
					string.String(), B_TRANSLATE("Cancel"),
					B_TRANSLATE("Open all"));
				alert->SetShortcut(0, B_ESCAPE);
				if (alert->Go() == 0)
					break;
			}
			message->AddPointer("window", this);
			be_app->PostMessage(message);
			break;
		}

		case B_SIMPLE_DATA:
		{
			const char* filetype = message->GetString("be:filetypes");
			if (filetype != NULL
				&& strcmp(filetype, "application/x-vnd.Be-bookmark") == 0
				&& LastMouseMovedView() == fBookmarkBar.get()) {
				// Something that can be made into a bookmark (e.g. the page icon)
				// was dragged and dropped on the bookmark bar.
				BPath path;
				if (_BookmarkPath(path) == B_OK && path.Append(kBookmarkBarSubdir) == B_OK) {
					entry_ref ref;
					if (BEntry(path.Path()).GetRef(&ref) != B_OK) {
						BAlert* alert = new BAlert(B_TRANSLATE("Bookmark error"),
							B_TRANSLATE("The bookmark could not be created."),
							B_TRANSLATE("OK"));
						alert->SetFlags(alert->Flags() | B_CLOSE_ON_ESCAPE);
						alert->Go(NULL);
						break;
					}
					message->AddRef("directory", &ref);
						// Add under the same name that Tracker would use, if
						// the ref had been added by dragging and dropping to Tracker.
					fBookmarkManager->CreateBookmark(this);
				}
				break;
			}

			// User possibly dropped files on this window.
			// If there is more than one entry_ref, let the app handle it
			// (open one new page per ref). If there is one ref, open it in
			// this window.
			type_code type;
			int32 countFound;
			if (message->GetInfo("refs", &type, &countFound) != B_OK
				|| type != B_REF_TYPE) {
				break;
			}
			if (countFound > 1) {
				message->what = B_REFS_RECEIVED;
				be_app->PostMessage(message);
				break;
			}
			entry_ref ref;
			if (message->FindRef("refs", &ref) != B_OK)
				break;
			BEntry entry(&ref, true);
			BPath path;
			if (!entry.Exists())
				break;
			if (entry.GetPath(&path) != B_OK) {
				BAlert* alert = new BAlert(B_TRANSLATE("Open file error"),
					B_TRANSLATE("The file could not be opened."),
					B_TRANSLATE("OK"));
				alert->SetFlags(alert->Flags() | B_CLOSE_ON_ESCAPE);
				alert->Go(NULL);
				break;
			}

			BUrl url(path);
			CurrentWebView()->LoadURL(url);
			break;
		}

		case ZOOM_FACTOR_INCREASE:
			CurrentWebView()->IncreaseZoomFactor(fZoomTextOnly);
			break;
		case ZOOM_FACTOR_DECREASE:
			CurrentWebView()->DecreaseZoomFactor(fZoomTextOnly);
			break;
		case ZOOM_FACTOR_RESET:
			CurrentWebView()->ResetZoomFactor();
			break;
		case ZOOM_TEXT_ONLY:
			fZoomTextOnly = !fZoomTextOnly;
			fMenuManager->ZoomTextOnlyMenuItem()->SetMarked(fZoomTextOnly);
			if (CurrentWebView())
				CurrentWebView()->Reload();
			break;

		case TOGGLE_FULLSCREEN:
			ToggleFullscreen();
			break;

		case TOGGLE_AUTO_HIDE_INTERFACE_IN_FULLSCREEN:
			_SetAutoHideInterfaceInFullscreen(
				!fAutoHideInterfaceInFullscreenMode);
			break;

		case CHECK_AUTO_HIDE_INTERFACE:
			_CheckAutoHideInterface();
			break;

		case SHOW_PAGE_SOURCE:
			CurrentWebView()->WebPage()->SendPageSource();
			break;
		case B_PAGE_SOURCE_RESULT:
			_HandlePageSourceResult(message);
			break;

		case EDIT_FIND_NEXT:
		case MSG_FIND_NEXT:
			CurrentWebView()->FindString(fFindView->Text(), true,
				fFindView->IsCaseSensitive());
			break;
		case MSG_FIND_TEXT_CHANGED:
		{
			bool findTextAvailable = strlen(fFindView->Text()) > 0;
	fMenuManager->FindPreviousMenuItem()->SetEnabled(false);
	fMenuManager->FindNextMenuItem()->SetEnabled(false);
			break;
		}
		case EDIT_FIND_PREVIOUS:
		case MSG_FIND_PREVIOUS:
			CurrentWebView()->FindString(fFindView->Text(), false,
				fFindView->IsCaseSensitive());
			break;
		case EDIT_SHOW_FIND_GROUP:
			if (fFindView->IsHidden())
				fFindView->Show();
			fFindView->MakeFocus(true);
			break;
		case EDIT_HIDE_FIND_GROUP:
		case MSG_FIND_CLOSED:
			if (!fFindView->IsHidden()) {
				fFindView->Hide();
				if (CurrentWebView() != NULL)
					CurrentWebView()->MakeFocus(true);
			}
			break;

		case B_CUT:
		case B_COPY:
		case B_PASTE:
		{
			BTextView* textView = dynamic_cast<BTextView*>(CurrentFocus());
			if (textView != NULL)
				textView->MessageReceived(message);
			else if (CurrentWebView() != NULL)
				CurrentWebView()->MessageReceived(message);
			break;
		}

		case B_EDITING_CAPABILITIES_RESULT:
		{
			BWebView* webView;
			if (message->FindPointer("view",
					reinterpret_cast<void**>(&webView)) != B_OK
				|| webView != CurrentWebView()) {
				break;
			}
			bool canCut;
			bool canCopy;
			bool canPaste;
			if (message->FindBool("can cut", &canCut) != B_OK)
				canCut = false;
			if (message->FindBool("can copy", &canCopy) != B_OK)
				canCopy = false;
			if (message->FindBool("can paste", &canPaste) != B_OK)
				canPaste = false;
			fMenuManager->CutMenuItem()->SetEnabled(canCut);
			fMenuManager->CopyMenuItem()->SetEnabled(canCopy);
			fMenuManager->PasteMenuItem()->SetEnabled(canPaste);
			break;
		}

		case SHOW_DOWNLOAD_WINDOW:
		case SHOW_SETTINGS_WINDOW:
		case SHOW_CONSOLE_WINDOW:
		case SHOW_COOKIE_WINDOW:
			message->AddUInt32("workspaces", Workspaces());
			be_app->PostMessage(message);
			break;

		case CLOSE_TAB:
			if (fTabManager->CountTabs() > 1) {
				int32 index;
				if (message->FindInt32("tab index", &index) != B_OK)
					index = fTabManager->SelectedTabIndex();
				_ShutdownTab(index);
				_UpdateTabGroupVisibility();
			} else
				PostMessage(B_QUIT_REQUESTED);
			break;

		case SELECT_TAB:
		{
			int32 index;
			if (message->FindInt32("tab index", &index) == B_OK
				&& fTabManager->SelectedTabIndex() != index
				&& fTabManager->CountTabs() > index) {
				fTabManager->SelectTab(index);
			}

			break;
		}

		case CYCLE_TABS:
		{
			int32 index = fTabManager->SelectedTabIndex() + 1;
			if (index >= fTabManager->CountTabs())
				index = 0;
			fTabManager->SelectTab(index);
		}
		break;

		case TAB_CHANGED:
		{
			// This message may be received also when the last tab closed,
			// i.e. with index == -1.
			int32 index;
			if (message->FindInt32("tab index", &index) != B_OK)
				index = -1;
			if (index == -1 && fTabManager->CountTabs() == 0)
				PostMessage(B_QUIT_REQUESTED);
			else
				_TabChanged(index);
		}
		break;

		case SETTINGS_VALUE_CHANGED:
		{
			BString name;
			if (message->FindString("name", &name) != B_OK)
				break;
			bool flag;
			BString string;
			uint32 value;
			if (name == kSettingsKeyShowTabsIfSinglePageOpen
				&& message->FindBool("value", &flag) == B_OK) {
				if (fShowTabsIfSinglePageOpen != flag) {
					fShowTabsIfSinglePageOpen = flag;
					_UpdateTabGroupVisibility();
				}
			} else if (name == kSettingsKeyAutoHidePointer
				&& message->FindBool("value", &flag) == B_OK) {
				fAutoHidePointer = flag;
				if (CurrentWebView())
					CurrentWebView()->SetAutoHidePointer(fAutoHidePointer);
			} else if (name == kSettingsKeyStartPageURL
				&& message->FindString("value", &string) == B_OK) {
				fStartPageURL = string;
			} else if (name == kSettingsKeySearchPageURL
				&& message->FindString("value", &string) == B_OK) {
				fSearchPageURL = string;
			} else if (name == kSettingsKeyNewWindowPolicy
				&& message->FindUInt32("value", &value) == B_OK) {
				fNewWindowPolicy = value;
			} else if (name == kSettingsKeyNewTabPolicy
				&& message->FindUInt32("value", &value) == B_OK) {
				fNewTabPolicy = value;
			} else if (name == kSettingsKeyAutoHideInterfaceInFullscreenMode
				&& message->FindBool("value", &flag) == B_OK) {
				_SetAutoHideInterfaceInFullscreen(flag);
			} else if (name == kSettingsKeyShowHomeButton
				&& message->FindBool("value", &flag) == B_OK) {
				if (flag)
					fHomeButton->Show();
				else
					fHomeButton->Hide();
			} else if (name == kSettingsShowBookmarkBar
				&& message->FindBool("value", &flag) == B_OK) {
				_ShowBookmarkBar(flag);
			}
		}
		break;

		case B_COPY_TARGET:
		{
			const char* filetype = message->GetString("be:filetypes");
			if (filetype != NULL && strcmp(filetype, "application/x-vnd.Be-bookmark") == 0) {
				// Tracker replied after the user dragged and dropped something
				// that can be bookmarked (e.g. the page icon) to a Tracker window.
				fBookmarkManager->CreateBookmark(this);
				break;
			}
		}
		// fall through
		default:
			BWebWindow::MessageReceived(message);
			break;
	}
}


status_t
BrowserWindow::Archive(BMessage* archive, bool deep) const
{
	status_t status = archive->AddRect("window frame", Frame());
	if (status == B_OK)
		status = archive->AddUInt32("window workspaces", Workspaces());

	for (int i = 0; i < fTabManager->CountTabs(); i++) {
		BWebView* view = dynamic_cast<BWebView*>(fTabManager->ViewForTab(i));
		if (view == NULL) {
			continue;
		}

		if (status == B_OK)
			status = archive->AddString("tab", view->MainFrameURL());
	}

	return status;
}


bool
BrowserWindow::QuitRequested()
{
	// TODO: Check for modified form data and ask user for confirmation, etc.

	BMessage message(WINDOW_CLOSED);
	Archive(&message);

	// Iterate over all tabs to delete all BWebViews.
	// Do this here, so WebKit tear down happens earlier.
	SetCurrentWebView(NULL);
	while (fTabManager->CountTabs() > 0)
		_ShutdownTab(0);

	message.AddRect("window frame", WindowFrame());
	be_app->PostMessage(&message);
	return true;
}


void
BrowserWindow::MenusBeginning()
{
	// Don't populate the history menu right away, as it can be slow.
	// Just add a placeholder and populate it when it's actually opened.
	for (int32 i = fMenuManager->HistoryMenu()->CountItems() - 1; i >= fMenuManager->HistoryMenuFixedItemCount(); i--) {
		BMenuItem* menuItem = fMenuManager->HistoryMenu()->RemoveItem(i);
		delete menuItem;
	}
	fMenuManager->HistoryMenu()->AddItem(new BMenuItem(B_TRANSLATE("Loading history" B_UTF8_ELLIPSIS), NULL));
	fMenuManager->HistoryMenu()->SetTargetForItems(this);
	fMenuManager->HistoryMenu()->SetTrackingHook(_HistoryMenuHook, this);

	_UpdateClipboardItems();
	fMenusRunning = true;
}


void
BrowserWindow::MenusEnded()
{
	fMenusRunning = false;
}


void
BrowserWindow::ScreenChanged(BRect screenSize, color_space format)
{
	if (fIsFullscreen)
		_ResizeToScreen();
}


void
BrowserWindow::WorkspacesChanged(uint32 oldWorkspaces, uint32 newWorkspaces)
{
	if (fIsFullscreen)
		_ResizeToScreen();
}


static bool
viewIsChild(const BView* parent, const BView* view)
{
	if (parent == view)
		return true;

	int32 count = parent->CountChildren();
	for (int32 i = 0; i < count; i++) {
		BView* child = parent->ChildAt(i);
		if (viewIsChild(child, view))
			return true;
	}
	return false;
}


void
BrowserWindow::SetCurrentWebView(BWebView* webView)
{
	if (webView == CurrentWebView())
		return;

	if (CurrentWebView() != NULL) {
		// Remember the currently focused view before switching tabs,
		// so that we can revert the focus when switching back to this tab
		// later.
		PageUserData* userData = static_cast<PageUserData*>(
			CurrentWebView()->GetUserData());
		if (userData == NULL) {
			userData = new(std::nothrow) PageUserData(CurrentFocus());
			if (userData == NULL)
				return;
			CurrentWebView()->SetUserData(userData);
		}
		userData->SetFocusedView(CurrentFocus());
		userData->SetURLInputContents(fURLInputGroup->Text());
		int32 selectionStart;
		int32 selectionEnd;
		fURLInputGroup->TextView()->GetSelection(&selectionStart,
			&selectionEnd);
		userData->SetURLInputSelection(selectionStart, selectionEnd);
	}

	BWebWindow::SetCurrentWebView(webView);

	if (webView != NULL) {
		webView->SetAutoHidePointer(fAutoHidePointer);

		_UpdateTitle(webView->MainFrameTitle());

		// Restore the previous focus or focus the web view.
		PageUserData* userData = static_cast<PageUserData*>(
			webView->GetUserData());
		BView* focusedView = NULL;
		if (userData != NULL)
			focusedView = userData->FocusedView();

		if (focusedView != NULL
			&& viewIsChild(GetLayout()->View(), focusedView)) {
			focusedView->MakeFocus(true);
		} else
			webView->MakeFocus(true);

		bool state = fURLInputGroup->IsURLInputLocked();
		fURLInputGroup->LockURLInput(false);
			// Unlock it so the following code can update the URL

		if (userData != NULL) {
			fURLInputGroup->SetPageIcon(userData->PageIcon());
			if (userData->URLInputContents().Length())
				fURLInputGroup->SetText(userData->URLInputContents());
			else
				fURLInputGroup->SetText(BString(webView->MainFrameURL()));
			if (userData->URLInputSelectionStart() >= 0) {
				fURLInputGroup->TextView()->Select(
					userData->URLInputSelectionStart(),
					userData->URLInputSelectionEnd());
			}
		} else {
			fURLInputGroup->SetPageIcon(NULL);
			fURLInputGroup->SetText(BString(webView->MainFrameURL()));
		}

		fURLInputGroup->LockURLInput(state);
			// Restore the state

		// Trigger update of the interface to the new page, by requesting
		// to resend all notifications.
		webView->WebPage()->ResendNotifications();
	} else
		_UpdateTitle("");
}


bool
BrowserWindow::IsBlankTab() const
{
	if (CurrentWebView() == NULL)
		return false;
	BString requestedURL = CurrentWebView()->MainFrameRequestedURL();
	return requestedURL.Length() == 0
		|| requestedURL == _NewTabURL(fTabManager->CountTabs() == 1);
}


void
BrowserWindow::CreateNewTab(const BString& _url, bool select,
	BWebView* webView)
{
	bool applyNewPagePolicy = webView == NULL;
	// Executed in app thread (new BWebPage needs to be created in app thread).
	if (webView == NULL)
		webView = new BWebView("web view", fContext);

	bool isNewWindow = fTabManager->CountTabs() == 0;

	fTabManager->AddTab(webView, B_TRANSLATE("New tab"));

	BString url(_url);
	if (applyNewPagePolicy && url.Length() == 0)
		url = _NewTabURL(isNewWindow);

	if (url.Length() > 0)
		webView->LoadURL(url.String());

	if (select) {
		fTabManager->SelectTab(fTabManager->CountTabs() - 1);
		SetCurrentWebView(webView);
		webView->WebPage()->ResendNotifications();
		fURLInputGroup->SetPageIcon(NULL);
		fURLInputGroup->SetText(url.String());
		fURLInputGroup->MakeFocus(true);
	}

	_ShowInterface(true);
	_UpdateTabGroupVisibility();
}


BRect
BrowserWindow::WindowFrame() const
{
	if (fIsFullscreen)
		return fNonFullscreenWindowFrame;
	else
		return Frame();
}


void
BrowserWindow::ToggleFullscreen()
{
	if (fIsFullscreen) {
		MoveTo(fNonFullscreenWindowFrame.LeftTop());
		ResizeTo(fNonFullscreenWindowFrame.Width(),
			fNonFullscreenWindowFrame.Height());

		SetFlags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
		SetLook(B_DOCUMENT_WINDOW_LOOK);

		_ShowInterface(true);
	} else {
		fNonFullscreenWindowFrame = Frame();
		_ResizeToScreen();

		SetFlags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
		SetLook(B_TITLED_WINDOW_LOOK);
	}
	fIsFullscreen = !fIsFullscreen;
	fMenuManager->FullscreenItem()->SetMarked(fIsFullscreen);
	fToggleFullscreenButton->SetVisible(fIsFullscreen);
}


// #pragma mark - Notification API


void
BrowserWindow::NavigationRequested(const BString& url, BWebView* view)
{
	if (CurrentWebView() == NULL)
		return;
}


void
BrowserWindow::NewWindowRequested(const BString& url, bool primaryAction)
{
	// Always open new windows in the application thread, since
	// creating a BWebView will try to grab the application lock.
	// But our own WebPage may already try to lock us from within
	// the application thread -> dead-lock. Thus we can't wait for
	// a reply here.
	BMessage message(NEW_TAB);
	message.AddPointer("window", this);
	message.AddString("url", url);
	message.AddBool("select", primaryAction);
	be_app->PostMessage(&message);
}


void
BrowserWindow::NewPageCreated(BWebView* view, BRect windowFrame,
	bool modalDialog, bool resizable, bool activate)
{
	if (windowFrame.IsValid()) {
		BrowserWindow* window = new BrowserWindow(windowFrame, fAppSettings,
			BString(), fContext, INTERFACE_ELEMENT_STATUS,
			view);
		window->Show();
	} else
		CreateNewTab(BString(), activate, view);
}


void
BrowserWindow::CloseWindowRequested(BWebView* view)
{
	int32 index = fTabManager->TabForView(view);
	if (index < 0) {
		// Tab is already gone.
		return;
	}
	BMessage message(CLOSE_TAB);
	message.AddInt32("tab index", index);
	PostMessage(&message, this);
}


void
BrowserWindow::LoadNegotiating(const BString& url, BWebView* view)
{
	if (CurrentWebView() == NULL)
		return;

	if (view != CurrentWebView()) {
		// Update the userData contents instead so the user sees
		// the correct URL when they switch back to that tab.
		PageUserData* userData = static_cast<PageUserData*>(
			view->GetUserData());
		if (userData != NULL && userData->URLInputContents().Length() == 0) {
			userData->SetURLInputContents(url);
		}
	}

	fURLInputGroup->SetText(url.String());

	BString status(B_TRANSLATE("Requesting %url"));
	status.ReplaceFirst("%url", url);
	view->WebPage()->SetStatusMessage(status);
}


void
BrowserWindow::LoadCommitted(const BString& url, BWebView* view)
{
	if (CurrentWebView() == NULL || view != CurrentWebView())
		return;

	// This hook is invoked when the load is committed.
	fURLInputGroup->SetText(url.String());

	BString status(B_TRANSLATE("Loading %url"));
	status.ReplaceFirst("%url", url);
	view->WebPage()->SetStatusMessage(status);
}


void
BrowserWindow::LoadProgress(float progress, BWebView* view)
{
	if (CurrentWebView() == NULL || view != CurrentWebView())
		return;

	if (progress < 100 && fLoadingProgressBar->IsHidden())
		_ShowProgressBar(true);
	else if (progress == 100 && !fLoadingProgressBar->IsHidden())
		_ShowProgressBar(false);
	fLoadingProgressBar->SetTo(progress);
}


void
BrowserWindow::LoadFailed(const BString& url, BWebView* view)
{
	if (CurrentWebView() == NULL || view != CurrentWebView())
		return;

	BString status(B_TRANSLATE_COMMENT("%url failed", "Loading URL failed. "
		"Don't translate variable %url."));
	status.ReplaceFirst("%url", url);
	view->WebPage()->SetStatusMessage(status);
	if (!fLoadingProgressBar->IsHidden())
		fLoadingProgressBar->Hide();
}


void
BrowserWindow::LoadFinished(const BString& url, BWebView* view)
{
	if (CurrentWebView() == NULL || view != CurrentWebView())
		return;

	fURLInputGroup->SetText(url.String());

	BString status(B_TRANSLATE_COMMENT("%url finished", "Loading URL "
		"finished. Don't translate variable %url."));
	status.ReplaceFirst("%url", url);
	view->WebPage()->SetStatusMessage(status);
	if (!fLoadingProgressBar->IsHidden())
		fLoadingProgressBar->Hide();

	NavigationCapabilitiesChanged(fBackButton->IsEnabled(),
		fForwardButton->IsEnabled(), false, view);

	int32 tabIndex = fTabManager->TabForView(view);
	if (tabIndex > 0 && strcmp(B_TRANSLATE("New tab"),
		fTabManager->TabLabel(tabIndex)) == 0)
			fTabManager->SetTabLabel(tabIndex, url);
}


void
BrowserWindow::MainDocumentError(const BString& failingURL,
	const BString& localizedDescription, BWebView* view)
{
	if (CurrentWebView() == NULL)
		return;

	// Make sure we show the page that contains the view.
	if (!_ShowPage(view))
		return;

	// Try delegating the URL to an external app instead.
	int32 at = failingURL.FindFirst(":");
	if (at > 0) {
		BString proto;
		failingURL.CopyInto(proto, 0, at);

		bool handled = false;

		for (unsigned int i = 0; i < sizeof(kHandledProtocols) / sizeof(char*);
				i++) {
			handled = (proto == kHandledProtocols[i]);
			if (handled)
				break;
		}

		if (!handled) {
			_SmartURLHandler(failingURL);
			return;
		}
	}

	BWebWindow::MainDocumentError(failingURL, localizedDescription, view);

	BrowsingHistory* history = BrowsingHistory::DefaultInstance();
	if (history->Lock()) {
		BObjectList<BrowsingHistoryItem> items;
		for (int32 i = 0; i < history->CountItems(); i++)
			items.AddItem(new BrowsingHistoryItem(history->HistoryItemAt(i)));

		history->Clear();
		for (int32 i = 0; i < items.CountItems(); i++) {
			BrowsingHistoryItem* item = items.ItemAt(i);
			if (item->URL() != failingURL)
				history->AddItem(*item);
		}
		history->Unlock();
	}
}


void
BrowserWindow::TitleChanged(const BString& title, BWebView* view)
{
	if (CurrentWebView() == NULL)
		return;

	int32 tabIndex = fTabManager->TabForView(view);
	if (tabIndex < 0)
		return;

	fTabManager->SetTabLabel(tabIndex, title);

	if (view != CurrentWebView())
		return;

	_UpdateTitle(title);
}


void
BrowserWindow::IconReceived(const BBitmap* icon, BWebView* view)
{
	if (CurrentWebView() == NULL)
		return;

	// The view may already be gone, since this notification arrives
	// asynchronously.
	if (!fTabManager->HasView(view))
		return;

	_SetPageIcon(view, icon);
}


void
BrowserWindow::ResizeRequested(float width, float height, BWebView* view)
{
	if (CurrentWebView() == NULL || view != CurrentWebView())
		return;

	// Ignore request when there is more than one BWebView embedded.
	if (fTabManager->CountTabs() > 1)
		return;

	// Make sure the new frame is not larger than the screen frame minus
	// window decorator border.
	BScreen screen(this);
	BRect screenFrame = screen.Frame();
	BRect decoratorFrame = DecoratorFrame();
	BRect frame = Frame();

	screenFrame.left += decoratorFrame.left - frame.left;
	screenFrame.right += decoratorFrame.right - frame.right;
	screenFrame.top += decoratorFrame.top - frame.top;
	screenFrame.bottom += decoratorFrame.bottom - frame.bottom;

	width = min_c(width, screen.Frame().Width());
	height = min_c(height, screen.Frame().Height());

	frame.right = frame.left + width;
	frame.bottom = frame.top + height;

	// frame is now not larger than screenFrame, but may still be partly outside
	if (!screenFrame.Contains(frame)) {
		if (frame.left < screenFrame.left)
			frame.OffsetBy(screenFrame.left - frame.left, 0);
		else if (frame.right > screenFrame.right)
			frame.OffsetBy(screenFrame.right - frame.right, 0);
		if (frame.top < screenFrame.top)
			frame.OffsetBy(screenFrame.top - frame.top, 0);
		else if (frame.bottom > screenFrame.bottom)
			frame.OffsetBy(screenFrame.bottom - frame.bottom, 0);
	}

	MoveTo(frame.left, frame.top);
	ResizeTo(width, height);
}


void
BrowserWindow::SetToolBarsVisible(bool flag, BWebView* view)
{
	if (CurrentWebView() == NULL || fTabManager->CountTabs() > 1)
		return;
}


void
BrowserWindow::SetStatusBarVisible(bool flag, BWebView* view)
{
	if (CurrentWebView() == NULL || fTabManager->CountTabs() > 1)
		return;
}


void
BrowserWindow::SetMenuBarVisible(bool flag, BWebView* view)
{
	if (CurrentWebView() == NULL || fTabManager->CountTabs() > 1)
		return;
}


void
BrowserWindow::SetResizable(bool flag, BWebView* view)
{
	if (CurrentWebView() == NULL || fTabManager->CountTabs() > 1)
		return;

	if (flag)
		SetFlags(Flags() & ~B_NOT_RESIZABLE);
	else
		SetFlags(Flags() | B_NOT_RESIZABLE);
}


void
BrowserWindow::StatusChanged(const BString& statusText, BWebView* view)
{
	if (CurrentWebView() == NULL || view != CurrentWebView())
		return;

	if (fStatusText)
		fStatusText->SetText(statusText.String());
}


void
BrowserWindow::NavigationCapabilitiesChanged(bool canGoBackward,
	bool canGoForward, bool canStop, BWebView* view)
{
	if (CurrentWebView() == NULL || view != CurrentWebView())
		return;

	fBackButton->SetEnabled(canGoBackward);
	fForwardButton->SetEnabled(canGoForward);
	fStopButton->SetEnabled(canStop);

	fMenuManager->BackMenuItem()->SetEnabled(canGoBackward);
	fMenuManager->ForwardMenuItem()->SetEnabled(canGoForward);
}


void
BrowserWindow::UpdateGlobalHistory(const BString& url)
{
	if (CurrentWebView() == NULL)
		return;
	BrowsingHistory::DefaultInstance()->AddItem(BrowsingHistoryItem(url));

	fURLInputGroup->SetText(CurrentWebView()->MainFrameURL());
}


bool
BrowserWindow::AuthenticationChallenge(BString message, BString& inOutUser,
	BString& inOutPassword, bool& inOutRememberCredentials,
	uint32 failureCount, BWebView* view)
{
	if (CurrentWebView() == NULL)
		return false;
	CredentialsStorage* persistentStorage
		= CredentialsStorage::PersistentInstance();
	CredentialsStorage* sessionStorage
		= CredentialsStorage::SessionInstance();

	// TODO: Using the message as key here is not so smart.
	HashString key(message);

	if (failureCount == 0) {
		if (persistentStorage->Contains(key)) {
			Credentials credentials = persistentStorage->GetCredentials(key);
			inOutUser = credentials.Username();
			inOutPassword = credentials.Password();
			return true;
		} else if (sessionStorage->Contains(key)) {
			Credentials credentials = sessionStorage->GetCredentials(key);
			inOutUser = credentials.Username();
			inOutPassword = credentials.Password();
			return true;
		}
	}
	// Switch to the page for which this authentication is required.
	if (!_ShowPage(view))
		return false;

	AuthenticationPanel* panel = new AuthenticationPanel(Frame());
		// Panel auto-destructs.
	bool success = panel->getAuthentication(message, inOutUser, inOutPassword,
		inOutRememberCredentials, failureCount > 0, inOutUser, inOutPassword,
		&inOutRememberCredentials);
	if (success) {
		Credentials credentials(inOutUser, inOutPassword);
		if (inOutRememberCredentials)
			persistentStorage->PutCredentials(key, credentials);
		else
			sessionStorage->PutCredentials(key, credentials);
	}
	return success;
}


// #pragma mark - private


bool
BrowserWindow::_HistoryMenuHook(BMenu* menu, void* userData)
{
	BrowserWindow* window = static_cast<BrowserWindow*>(userData);
	window->_PopulateHistoryMenu();
	// Remove the hook so it doesn't get called again.
	menu->SetTrackingHook(NULL, NULL);
	return false;
}


void
BrowserWindow::_UpdateTitle(const BString& title)
{
	BString windowTitle;

	if (title.Length() > 0)
		windowTitle = title;
	else {
		BWebView* webView = CurrentWebView();
		if (webView != NULL) {
			BString url = webView->MainFrameURL();
			int32 leafPos = url.FindLast('/');
			url.Remove(0, leafPos + 1);
			windowTitle = url;
		}
	}

	if (windowTitle.Length() > 0)
		windowTitle << " - ";
	windowTitle << kApplicationName;
	SetTitle(windowTitle.String());
}


void
BrowserWindow::_UpdateTabGroupVisibility()
{
	if (Lock()) {
		if (fInterfaceVisible)
			fTabGroup->SetVisible(_TabGroupShouldBeVisible());
		fTabManager->SetCloseButtonsAvailable(fTabManager->CountTabs() > 1);
		Unlock();
	}
}


bool
BrowserWindow::_TabGroupShouldBeVisible() const
{
	return (fShowTabsIfSinglePageOpen || fTabManager->CountTabs() > 1)
		&& (fVisibleInterfaceElements & INTERFACE_ELEMENT_TABS) != 0;
}


void
BrowserWindow::_ShutdownTab(int32 index)
{
	BView* view = fTabManager->RemoveTab(index);
	BWebView* webView = dynamic_cast<BWebView*>(view);
	if (webView == CurrentWebView())
		SetCurrentWebView(NULL);
	if (webView != NULL)
		webView->Shutdown();
	else
		delete view;
}


void
BrowserWindow::_TabChanged(int32 index)
{
	SetCurrentWebView(dynamic_cast<BWebView*>(fTabManager->ViewForTab(index)));
}


status_t
BrowserWindow::_BookmarkPath(BPath& path) const
{
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK)
		return B_ERROR;

	if (path.Append(kApplicationName) != B_OK)
		return B_ERROR;

	return path.Append("Bookmarks");
}




void
BrowserWindow::_SetPageIcon(BWebView* view, const BBitmap* icon)
{
	PageUserData* userData = static_cast<PageUserData*>(view->GetUserData());
	if (userData == NULL) {
		userData = new(std::nothrow) PageUserData(NULL);
		if (userData == NULL)
			return;
		view->SetUserData(userData);
	}
	// The PageUserData makes a copy of the icon, which we pass on to
	// the TabManager for display in the respective tab.
	userData->SetPageIcon(icon);
	fTabManager->SetTabIcon(view, userData->PageIcon());
	if (view == CurrentWebView())
		fURLInputGroup->SetPageIcon(icon);
}




void
BrowserWindow::_PopulateHistoryMenu()
{
	BMenuItem* item;
	while ((item = fMenuManager->HistoryMenu()->RemoveItem(fMenuManager->HistoryMenuFixedItemCount())) != NULL)
		delete item;

	if (!fHistoryItems)
		return;

	int32 count = fHistoryItems->CountItems();
	BMenuItem* clearHistoryItem = new BMenuItem(B_TRANSLATE("Clear history"),
		new BMessage(CLEAR_HISTORY));
	clearHistoryItem->SetEnabled(count > 0);
	fMenuManager->HistoryMenu()->AddItem(clearHistoryItem);
	if (count == 0)
		return;

	fMenuManager->HistoryMenu()->AddSeparatorItem();

	int32 maxCount = min_c(count, 20);
	for (int32 i = 0; i < maxCount; i++) {
		BrowsingHistoryItem* historyItem = fHistoryItems->ItemAt(i);
		BMessage* message = new BMessage(GOTO_URL);
		message->AddString("url", historyItem->URL());

		BString truncatedUrl(historyItem->URL());
		be_plain_font->TruncateString(&truncatedUrl, B_TRUNCATE_END, 480);
		BMenuItem* menuItem = new BMenuItem(truncatedUrl, message);
		menuItem->SetTarget(this);
		fMenuManager->HistoryMenu()->AddItem(menuItem);
	}

	fMenuManager->HistoryMenu()->AddSeparatorItem();
	fMenuManager->HistoryMenu()->AddItem(new BMenuItem(B_TRANSLATE("Show all history"),
		new BMessage(SHOW_HISTORY_WINDOW)));
}


void
BrowserWindow::_UpdateClipboardItems()
{
	if (CurrentWebView() == NULL)
		return;

	BTextView* focusTextView = dynamic_cast<BTextView*>(CurrentFocus());
	if (focusTextView != NULL) {
		int32 selectionStart;
		int32 selectionEnd;
		focusTextView->GetSelection(&selectionStart, &selectionEnd);
		bool hasSelection = selectionStart < selectionEnd;
		bool canPaste = false;
		// A BTextView has the focus.
		if (be_clipboard->Lock()) {
			bigtime_t lastModified = be_clipboard->LocalCount();
			if (lastModified > fLastClipboardCheckTime) {
				fLastClipboardCheckTime = lastModified;
				BMessage* data = be_clipboard->Data();
				if (data != NULL)
					canPaste = data->HasData("text/plain", B_MIME_TYPE);
			}
			be_clipboard->Unlock();
		}
		fMenuManager->CutMenuItem()->SetEnabled(hasSelection);
		fMenuManager->CopyMenuItem()->SetEnabled(hasSelection);
		fMenuManager->PasteMenuItem()->SetEnabled(canPaste);
	} else if (CurrentWebView() != NULL) {
		// Trigger update of the clipboard items, even if the
		// BWebView doesn't have focus, we'll dispatch these message
		// there anyway. This works so fast that the user can never see
		// the wrong enabled state when the menu opens until the result
		// message arrives. The initial state needs to be enabled, since
		// standard shortcut handling is always wrapped inside MenusBeginning()
		// and MenusEnded(), and since we update items asynchronously, we need
		// to have them enabled to begin with.
		fMenuManager->CutMenuItem()->SetEnabled(true);
		fMenuManager->CopyMenuItem()->SetEnabled(true);
		fMenuManager->PasteMenuItem()->SetEnabled(true);

		CurrentWebView()->WebPage()->SendEditingCapabilities();
	}
}


bool
BrowserWindow::_ShowPage(BWebView* view)
{
	if (view != CurrentWebView()) {
		int32 tabIndex = fTabManager->TabForView(view);
		if (tabIndex < 0) {
			// Page seems to be gone already?
			return false;
		}
		if (fTabManager->SelectedTabIndex() == tabIndex)
			return true;
		fTabManager->SelectTab(tabIndex);
		_TabChanged(tabIndex);
		UpdateIfNeeded();
	}
	return true;
}


void
BrowserWindow::_ResizeToScreen()
{
	BScreen screen(this);
	BRect decoratorFrame = DecoratorFrame();
	BRect frame = Frame();
	MoveTo(screen.Frame().left - (decoratorFrame.left - frame.left),
		screen.Frame().top - (decoratorFrame.top - frame.top));
	ResizeTo(screen.Frame().Width() - (decoratorFrame.Width() - frame.Width()),
		screen.Frame().Height() - (decoratorFrame.Height() - frame.Height()));
}


void
BrowserWindow::_SetAutoHideInterfaceInFullscreen(bool doIt)
{
	if (fAutoHideInterfaceInFullscreenMode == doIt)
		return;

	fAutoHideInterfaceInFullscreenMode = doIt;
	if (fAppSettings->GetValue(kSettingsKeyAutoHideInterfaceInFullscreenMode,
			doIt) != doIt) {
		fAppSettings->SetValue(kSettingsKeyAutoHideInterfaceInFullscreenMode,
			doIt);
	}

	if (fAutoHideInterfaceInFullscreenMode)
		fPulseRunner = std::make_unique<BMessageRunner>(BMessenger(this),
			new BMessage(CHECK_AUTO_HIDE_INTERFACE), 100000);
	else {
		fPulseRunner.reset();
		_ShowInterface(true);
	}
}


void
BrowserWindow::_CheckAutoHideInterface()
{
	if (!fIsFullscreen || !fAutoHideInterfaceInFullscreenMode
		|| (CurrentWebView() != NULL && !CurrentWebView()->IsFocus())) {
		return;
	}

	if (fLastMousePos.y == 0)
		_ShowInterface(true);
	else if (fNavigationGroup->IsVisible()
		&& fLastMousePos.y > fNavigationGroup->Frame().bottom
		&& system_time() - fLastMouseMovedTime > fAppSettings->GetValue("auto_hide_timeout", 1000000)) {
		// NOTE: Do not re-use navigationGroupBottom in the above
		// check, since we only want to hide the interface when it is visible.
		_ShowInterface(false);
	}
}


void
BrowserWindow::_ShowInterface(bool show)
{
	if (fInterfaceVisible == show)
		return;

	fInterfaceVisible = show;

	BView* container = FindView("interface_container");
	if (!container)
		return;

	if (show) {
		container->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
#if !INTEGRATE_MENU_INTO_TAB_BAR
		fMenuGroup->SetVisible(
			(fVisibleInterfaceElements & INTERFACE_ELEMENT_MENU) != 0);
#endif
		fTabGroup->SetVisible(_TabGroupShouldBeVisible());
		fNavigationGroup->SetVisible(
			(fVisibleInterfaceElements & INTERFACE_ELEMENT_NAVIGATION) != 0);
		fStatusGroup->SetVisible(
			(fVisibleInterfaceElements & INTERFACE_ELEMENT_STATUS) != 0);
	} else {
		rgb_color color = ui_color(B_PANEL_BACKGROUND_COLOR);
		color.alpha = 0;
		container->SetViewColor(color);
	}
	// TODO: Setting the group visible seems to unhide the status bar.
	// Fix in Haiku?
	while (!fLoadingProgressBar->IsHidden())
		fLoadingProgressBar->Hide();
}


void
BrowserWindow::_ShowProgressBar(bool show)
{
	if (show) {
		if (!fStatusGroup->IsVisible() && (fVisibleInterfaceElements
			& INTERFACE_ELEMENT_STATUS) != 0)
				fStatusGroup->SetVisible(true);
		fLoadingProgressBar->Show();
	} else {
		if (!fInterfaceVisible)
			fStatusGroup->SetVisible(false);
		// TODO: This is also used in _ShowInterface. Without it the status bar
		// doesn't always hide again. It may be an Interface Kit bug.
		while (!fLoadingProgressBar->IsHidden())
			fLoadingProgressBar->Hide();
	}
}


void
BrowserWindow::_InvokeButtonVisibly(BButton* button)
{
	button->SetValue(B_CONTROL_ON);
	UpdateIfNeeded();
	button->Invoke();
	snooze(1000);
	button->SetValue(B_CONTROL_OFF);
}


BString
BrowserWindow::_NewTabURL(bool isNewWindow) const
{
	BString url;
	uint32 policy = isNewWindow ? fNewWindowPolicy : fNewTabPolicy;
	// Implement new page policy
	switch (policy) {
		case OpenStartPage:
			url = fStartPageURL;
			break;
		case OpenSearchPage:
			url.SetTo(fSearchPageURL);
			url.ReplaceAll("%s", "");
			break;
		case CloneCurrentPage:
			if (CurrentWebView() != NULL)
				url = CurrentWebView()->MainFrameURL();
			break;
		case OpenBlankPage:
		default:
			break;
	}
	return url;
}


BString
BrowserWindow::_EncodeURIComponent(const BString& search)
{
	static const char* hex = "0123456789ABCDEF";
	BString result;
	for (int32 i = 0; i < search.Length(); i++) {
		char c = search[i];
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
			result += c;
		else {
			result += '%';
			result += hex[c >> 4];
			result += hex[c & 0xf];
		}
	}
	return result;
}


void
BrowserWindow::_VisitURL(const BString& url)
{
	// fURLInputGroup->TextView()->SetText(url);
	CurrentWebView()->LoadURL(url.String());
}


void
BrowserWindow::_VisitSearchEngine(const BString& search)
{
	BString searchQuery = search;

	BString searchPrefix;
	search.CopyCharsInto(searchPrefix, 0, 2);

	// Default search URL
	BString engine(fSearchPageURL);

	// Check if the string starts with one of the search engine shortcuts
	for (int i = 0; kSearchEngines[i].url != NULL; i++) {
		if (kSearchEngines[i].shortcut == searchPrefix) {
			engine = kSearchEngines[i].url;
			searchQuery.Remove(0, 2);
			break;
		}
	}

	engine.ReplaceAll("%s", _EncodeURIComponent(searchQuery));
	_VisitURL(engine);
}


/*! \brief "smart" parser for user-entered URLs

	We try to be flexible in what we accept as a valid URL. The protocol may
	be missing, or something we can't handle (in that case we run the matching
	app). If all attempts to make sense of the input fail, we make a search
	engine query for it.
 */
void
BrowserWindow::_SmartURLHandler(const BString& url)
{
	BUrl urlObject(url.String(), true);
	if (urlObject.Protocol().Length() > 0) {
		// This is a URL with a protocol. Let's see if we can handle it.
		bool handled = false;
		for (unsigned int i = 0; i < sizeof(kHandledProtocols) / sizeof(char*);
				i++) {
			if (urlObject.Protocol() == kHandledProtocols[i]) {
				handled = true;
				break;
			}
		}

		if (handled) {
			if (static_cast<BrowserApp*>(be_app)->GetBlocklistManager()->IsBlocked(urlObject))
				return;
			_VisitURL(url);
			return;
		} else {
			// There is what looks like a protocol, but one we don't know.
			// Ask the BRoster if there is a matching filetype and app which
			// can handle it.
			BString temp;
			temp = "application/x-vnd.Be.URL.";
			temp += urlObject.Protocol();

			const char* argv[] = { url.String(), NULL };

			if (be_roster->Launch(temp.String(), 1, argv) == B_OK)
				return;
		}
	}

	// If there is no protocol, check for a dot.
	if (url.FindFirst('.') >= 0) {
		BString urlWithHttp = BString("http://").Append(url);
		_VisitURL(urlWithHttp);
		return;
	}

	// If all else fails, treat it as a search query.
	_VisitSearchEngine(url);
}


void
BrowserWindow::_HandlePageSourceResult(const BMessage* message)
{
	BString source;
	status_t ret = message->FindString("source", &source);

	if (ret != B_OK) {
		char buffer[1024];
		snprintf(buffer, sizeof(buffer), "Failed to get the "
			"page source: %s\n", strerror(ret));
		BAlert* alert = new BAlert(B_TRANSLATE("Page source error"), buffer,
			B_TRANSLATE("OK"));
		alert->SetFlags(alert->Flags() | B_CLOSE_ON_ESCAPE);
		alert->Go(NULL);
		return;
	}

	BString title(B_TRANSLATE("Page Source"));
	title << " - " << CurrentWebView()->MainFrameTitle();

	SourceWindow* window = new SourceWindow(Frame().OffsetByCopy(20, 20),
		title, source);
	window->Show();
}


void
BrowserWindow::_ShowBookmarkBar(bool show)
{
	if (show) {
		BPath path;
		entry_ref ref;
		if (_BookmarkPath(path) == B_OK
			&& path.Append(kBookmarkBarSubdir) == B_OK) {
			if (get_ref_for_path(path.Path(), &ref) != B_OK) {
				BAlert* alert = new BAlert(B_TRANSLATE("Bookmark bar error"),
					B_TRANSLATE("The bookmark bar folder could not be found."),
					B_TRANSLATE("OK"));
				alert->SetFlags(alert->Flags() | B_CLOSE_ON_ESCAPE);
				alert->Go(NULL);
				fMenuManager->BookmarkBarMenuItem()->SetMarked(false);
				return;
			}
			BDirectory dir(&ref);
			if (dir.CountEntries() == 0) {
				fMenuManager->BookmarkBarMenuItem()->SetMarked(false);
				return;
			}
		}
	}

	fMenuManager->BookmarkBarMenuItem()->SetMarked(show);

	if (fBookmarkBar == NULL || fBookmarkBar->IsHidden() != show)
		return;

	fAppSettings->SetValue(kSettingsShowBookmarkBar, show);

	if (show)
		fBookmarkBar->Show();
	else
		fBookmarkBar->Hide();
}
