/*
 * Copyright 2010 Stephan Aßmus <superstippi@gmx.de>
 * Copyright 2019, Haiku, Inc.
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#include "SettingsWindow.h"

#include <Button.h>
#include <CheckBox.h>
#include <ControlLook.h>
#include <FilePanel.h>
#include <GridLayoutBuilder.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <LayoutBuilder.h>
#include <Locale.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <Message.h>
#include <PopUpMenu.h>
#include <ScrollView.h>
#include <SeparatorView.h>
#include <SpaceLayoutItem.h>
#include <Spinner.h>
#include <TabView.h>
#include <TextControl.h>
#include <debugger.h>
#include <SettingsMessage.h>
#include <Variant.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>

#include "BrowserApp.h"
#include "BrowsingHistory.h"
#include "BrowserWindow.h"
#include "FontSelectionView.h"
#include "SettingsKeys.h"
#include "WebSettings.h"


struct Setting {
	BControl* control;
	const char* key;
	type_code type;
	BVariant defaultValue;
};


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Settings Window"

enum {
	MSG_APPLY									= 'aply',
	MSG_CANCEL									= 'cncl',
	MSG_REVERT									= 'rvrt',

	MSG_START_PAGE_CHANGED						= 'hpch',
	MSG_SEARCH_PAGE_CHANGED						= 'spch',
	MSG_SEARCH_PAGE_CHANGED_MENU				= 'spcm',
	MSG_DOWNLOAD_FOLDER_CHANGED					= 'dnfc',
	MSG_NEW_WINDOWS_BEHAVIOR_CHANGED			= 'nwbc',
	MSG_NEW_TABS_BEHAVIOR_CHANGED				= 'ntbc',
	MSG_START_UP_BEHAVIOR_CHANGED				= 'subc',
	MSG_HISTORY_MENU_DAYS_CHANGED				= 'digm',
	MSG_TAB_DISPLAY_BEHAVIOR_CHANGED			= 'tdbc',
	MSG_AUTO_HIDE_INTERFACE_BEHAVIOR_CHANGED	= 'ahic',
	MSG_AUTO_HIDE_POINTER_BEHAVIOR_CHANGED		= 'ahpc',
	MSG_SHOW_HOME_BUTTON_CHANGED				= 'shbc',

	MSG_STANDARD_FONT_CHANGED					= 'stfc',
	MSG_SERIF_FONT_CHANGED						= 'sefc',
	MSG_SANS_SERIF_FONT_CHANGED					= 'ssfc',
	MSG_FIXED_FONT_CHANGED						= 'ffch',

	MSG_STANDARD_FONT_SIZE_SELECTED				= 'sfss',
	MSG_FIXED_FONT_SIZE_SELECTED				= 'ffss',

	MSG_USE_PROXY_CHANGED						= 'upsc',
	MSG_PROXY_ADDRESS_CHANGED					= 'psac',
	MSG_PROXY_PORT_CHANGED						= 'pspc',
	MSG_USE_PROXY_AUTH_CHANGED					= 'upsa',
	MSG_PROXY_USERNAME_CHANGED					= 'psuc',
	MSG_PROXY_PASSWORD_CHANGED					= 'pswc',

	MSG_CHOOSE_DOWNLOAD_FOLDER					= 'swop',
	MSG_HANDLE_DOWNLOAD_FOLDER					= 'oprs',
};

static const int32 kDefaultFontSize = 14;


SettingsWindow::SettingsWindow(BRect frame, SettingsMessage* settings)
	:
	BWindow(frame, B_TRANSLATE("Settings"), B_TITLED_WINDOW_LOOK,
		B_NORMAL_WINDOW_FEEL, B_AUTO_UPDATE_SIZE_LIMITS
			| B_ASYNCHRONOUS_CONTROLS | B_NOT_ZOOMABLE),
	fSettings(settings),
	fProxyPortValid(true),
	fStartPageValid(true),
	fSearchPageValid(true),
	fProxyAddressValid(true)
{
	fApplyButton = new BButton(B_TRANSLATE("Apply"), new BMessage(MSG_APPLY));
	fCancelButton = new BButton(B_TRANSLATE("Cancel"),
		new BMessage(MSG_CANCEL));
	fRevertButton = new BButton(B_TRANSLATE("Revert"),
		new BMessage(MSG_REVERT));

	fOpenFilePanel = NULL;

	float spacing = be_control_look->DefaultItemSpacing();

	BTabView* tabView = new BTabView("settings pages", B_WIDTH_FROM_LABEL);
	tabView->SetBorder(B_NO_BORDER);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(0, B_USE_DEFAULT_SPACING, 0, B_USE_WINDOW_SPACING)
		.Add(tabView)
		.Add(new BSeparatorView(B_HORIZONTAL))
		.AddGroup(B_HORIZONTAL)
			.SetInsets(B_USE_WINDOW_SPACING, B_USE_DEFAULT_SPACING,
				B_USE_WINDOW_SPACING, 0)
			.Add(fRevertButton)
			.AddGlue()
			.Add(fCancelButton)
			.Add(fApplyButton);

	tabView->AddTab(_CreateGeneralPage(spacing));
	tabView->AddTab(_CreateFontsPage(spacing));
	tabView->AddTab(_CreateProxyPage(spacing));

	_SetupFontSelectionView(fStandardFontView,
		new BMessage(MSG_STANDARD_FONT_CHANGED));
	_SetupFontSelectionView(fSerifFontView,
		new BMessage(MSG_SERIF_FONT_CHANGED));
	_SetupFontSelectionView(fSansSerifFontView,
		new BMessage(MSG_SANS_SERIF_FONT_CHANGED));
	_SetupFontSelectionView(fFixedFontView,
		new BMessage(MSG_FIXED_FONT_CHANGED));

	fApplyButton->MakeDefault(true);

	if (!frame.IsValid())
		CenterOnScreen();

	// load settings from disk
	_RevertSettings();
	// apply to WebKit
	_ApplySettings();

	// Start hidden
	Hide();
	Show();

	fSettingsCount = 5;
	fSettingsData = new Setting[fSettingsCount];

	fSettingsData[0] = Setting{ fStartPageControl, kSettingsKeyStartPageURL, B_STRING_TYPE, BVariant(kDefaultStartPageURL) };
	fSettingsData[1] = Setting{ fSearchPageControl, kSettingsKeySearchPageURL, B_STRING_TYPE, BVariant(kDefaultSearchPageURL) };
	fSettingsData[2] = Setting{ fDownloadFolderControl, kSettingsKeyDownloadPath, B_STRING_TYPE, BVariant(kDefaultDownloadPath) };
	fSettingsData[3] = Setting{ fShowTabsIfOnlyOnePage, kSettingsKeyShowTabsIfSinglePageOpen, B_BOOL_TYPE, BVariant(true) };
	fSettingsData[4] = Setting{ fAutoHideInterfaceInFullscreenMode, kSettingsKeyAutoHideInterfaceInFullscreenMode, B_BOOL_TYPE, BVariant(false) };
}


SettingsWindow::~SettingsWindow()
{
	delete[] fSettingsData;
	RemoveHandler(fStandardFontView);
	delete fStandardFontView;
	RemoveHandler(fSerifFontView);
	delete fSerifFontView;
	RemoveHandler(fSansSerifFontView);
	delete fSansSerifFontView;
	RemoveHandler(fFixedFontView);
	delete fFixedFontView;
	delete fOpenFilePanel;
}


void
SettingsWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case B_COLORS_UPDATED:
			fStandardFontView->MessageReceived(message);
			fSerifFontView->MessageReceived(message);
			fSansSerifFontView->MessageReceived(message);
			fFixedFontView->MessageReceived(message);
			break;
		case MSG_APPLY:
			_ApplySettings();
			break;
		case MSG_CANCEL:
			_RevertSettings();
			PostMessage(B_QUIT_REQUESTED);
			break;
		case MSG_REVERT:
			_RevertSettings();
			break;
		case MSG_CHOOSE_DOWNLOAD_FOLDER:
			_ChooseDownloadFolder(message);
			break;
		case MSG_HANDLE_DOWNLOAD_FOLDER:
			_HandleDownloadPanelResult(fOpenFilePanel, message);
			break;
		case MSG_STANDARD_FONT_SIZE_SELECTED:
		{
			int32 size = fStandardSizesSpinner->Value();
			fStandardFontView->SetSize(size);
			fSerifFontView->SetSize(size);
			fSansSerifFontView->SetSize(size);
			_ValidateControlsEnabledStatus();
			break;
		}
		case MSG_FIXED_FONT_SIZE_SELECTED:
		{
			int32 size = fFixedSizesSpinner->Value();
			fFixedFontView->SetSize(size);
			_ValidateControlsEnabledStatus();
			break;
		}

		case MSG_SEARCH_PAGE_CHANGED_MENU:
		{
			BString searchString;
			BMenuItem* source;
			if (message->FindString("searchstring", &searchString) == B_OK) {
				fSearchPageControl->SetText(searchString);
				fSearchPageControl->SetEnabled(false);
			} else
				fSearchPageControl->SetEnabled(true);

			if (message->FindPointer("source", (void**)&source) == B_OK)
				source->SetMarked(true);

			_ValidateControlsEnabledStatus();
			break;
		}

		case MSG_DOWNLOAD_FOLDER_CHANGED:
		case MSG_START_UP_BEHAVIOR_CHANGED:
		case MSG_NEW_WINDOWS_BEHAVIOR_CHANGED:
		case MSG_NEW_TABS_BEHAVIOR_CHANGED:
		case MSG_HISTORY_MENU_DAYS_CHANGED:
		case MSG_TAB_DISPLAY_BEHAVIOR_CHANGED:
		case MSG_AUTO_HIDE_INTERFACE_BEHAVIOR_CHANGED:
		case MSG_AUTO_HIDE_POINTER_BEHAVIOR_CHANGED:
		case MSG_SHOW_HOME_BUTTON_CHANGED:
			_ValidateControlsEnabledStatus();
			break;
		case MSG_START_PAGE_CHANGED:
			_ValidateStartPage();
			_ValidateControlsEnabledStatus();
			break;
		case MSG_SEARCH_PAGE_CHANGED:
			_ValidateSearchPage();
			_ValidateControlsEnabledStatus();
			break;
		case MSG_STANDARD_FONT_CHANGED:
		case MSG_SERIF_FONT_CHANGED:
		case MSG_SANS_SERIF_FONT_CHANGED:
		case MSG_FIXED_FONT_CHANGED:
		case MSG_USE_PROXY_CHANGED:
		case MSG_USE_PROXY_AUTH_CHANGED:
		case MSG_PROXY_USERNAME_CHANGED:
		case MSG_PROXY_PASSWORD_CHANGED:
			_ValidateControlsEnabledStatus();
			break;

		case MSG_PROXY_PORT_CHANGED:
			_ValidateProxyPort();
			_ValidateControlsEnabledStatus();
			break;
		case MSG_PROXY_ADDRESS_CHANGED:
			_ValidateProxyAddress();
			_ValidateControlsEnabledStatus();
			break;

		default:
			BWindow::MessageReceived(message);
			break;
	}
}


bool
SettingsWindow::QuitRequested()
{
	if (!IsHidden())
		Hide();
	return false;
}


void
SettingsWindow::Show()
{
	// When showing the window, this is always the
	// point to which we can revert the settings.
	_RevertSettings();
	BWindow::Show();
}


// #pragma mark - private


BView*
SettingsWindow::_CreateGeneralPage(float spacing)
{
	fStartPageControl = new BTextControl("start page",
		B_TRANSLATE("Start page:"), "", new BMessage(MSG_START_PAGE_CHANGED));
	fStartPageControl->SetModificationMessage(
		new BMessage(MSG_START_PAGE_CHANGED));
	fStartPageControl->SetText(
		fSettings->GetValue(kSettingsKeyStartPageURL, kDefaultStartPageURL));

	fSearchPageControl = new BTextControl("search page", "", "",
		new BMessage(MSG_SEARCH_PAGE_CHANGED));
	fSearchPageControl->SetModificationMessage(
		new BMessage(MSG_SEARCH_PAGE_CHANGED));
	BString searchURL = fSettings->GetValue(kSettingsKeySearchPageURL,
		kDefaultSearchPageURL);
	fSearchPageControl->SetText(searchURL);

	fDownloadFolderControl = new BTextControl("download folder",
		B_TRANSLATE("Download folder:"), "",
		new BMessage(MSG_DOWNLOAD_FOLDER_CHANGED));
	fDownloadFolderControl->SetModificationMessage(
		new BMessage(MSG_DOWNLOAD_FOLDER_CHANGED));
	fDownloadFolderControl->SetText(
		fSettings->GetValue(kSettingsKeyDownloadPath, kDefaultDownloadPath));

	fStartUpBehaviorResumePriorSession = new BMenuItem(
		B_TRANSLATE("Resume prior session"),
		new BMessage(MSG_START_UP_BEHAVIOR_CHANGED));
	fStartUpBehaviorStartNewSession = new BMenuItem(
		B_TRANSLATE("Start new session"),
		new BMessage(MSG_START_UP_BEHAVIOR_CHANGED));

	fNewWindowBehaviorOpenHomeItem = new BMenuItem(
		B_TRANSLATE("Open start page"),
		new BMessage(MSG_NEW_WINDOWS_BEHAVIOR_CHANGED));
	fNewWindowBehaviorOpenSearchItem = new BMenuItem(
		B_TRANSLATE("Open search page"),
		new BMessage(MSG_NEW_WINDOWS_BEHAVIOR_CHANGED));
	fNewWindowBehaviorOpenBlankItem = new BMenuItem(
		B_TRANSLATE("Open blank page"),
		new BMessage(MSG_NEW_WINDOWS_BEHAVIOR_CHANGED));

	fNewTabBehaviorCloneCurrentItem = new BMenuItem(
		B_TRANSLATE("Clone current page"),
		new BMessage(MSG_NEW_TABS_BEHAVIOR_CHANGED));
	fNewTabBehaviorOpenHomeItem = new BMenuItem(
		B_TRANSLATE("Open start page"),
		new BMessage(MSG_NEW_TABS_BEHAVIOR_CHANGED));
	fNewTabBehaviorOpenSearchItem = new BMenuItem(
		B_TRANSLATE("Open search page"),
		new BMessage(MSG_NEW_TABS_BEHAVIOR_CHANGED));
	fNewTabBehaviorOpenBlankItem = new BMenuItem(
		B_TRANSLATE("Open blank page"),
		new BMessage(MSG_NEW_TABS_BEHAVIOR_CHANGED));
	fChooseButton = new BButton(B_TRANSLATE("Browse" B_UTF8_ELLIPSIS),
		new BMessage(MSG_CHOOSE_DOWNLOAD_FOLDER));

	fNewWindowBehaviorOpenHomeItem->SetMarked(true);
	fNewTabBehaviorOpenBlankItem->SetMarked(true);
	fStartUpBehaviorResumePriorSession->SetMarked(true);

	BMenuItem* searchPageCustom = new BMenuItem(B_TRANSLATE("Custom"),
		new BMessage(MSG_SEARCH_PAGE_CHANGED_MENU));
	searchPageCustom->SetMarked(true);

	BPopUpMenu* searchPageMenu = new BPopUpMenu("Search page:");
	searchPageMenu->SetRadioMode(true);

	for (int i = 0; kSearchEngines[i].url != NULL; i++) {
		BMessage* message = new BMessage(MSG_SEARCH_PAGE_CHANGED_MENU);
		message->AddString("searchstring", kSearchEngines[i].url);
		searchPageMenu->AddItem(new BMenuItem(kSearchEngines[i].name, message));

	}
	searchPageMenu->AddItem(new BSeparatorItem());
	searchPageMenu->AddItem(searchPageCustom);
	fSearchPageMenu = new BMenuField("search page",
		B_TRANSLATE("Search page:"), searchPageMenu);
	fSearchPageMenu->SetToolTip(B_TRANSLATE("%s - Search term"));

	BPopUpMenu* startUpBehaviorMenu = new BPopUpMenu("Start up");
	startUpBehaviorMenu->AddItem(fStartUpBehaviorResumePriorSession);
	startUpBehaviorMenu->AddItem(fStartUpBehaviorStartNewSession);
	fStartUpBehaviorMenu = new BMenuField("start up behavior",
		B_TRANSLATE("Start up:"), startUpBehaviorMenu);


	BPopUpMenu* newWindowBehaviorMenu = new BPopUpMenu("New windows");
	newWindowBehaviorMenu->AddItem(fNewWindowBehaviorOpenHomeItem);
	newWindowBehaviorMenu->AddItem(fNewWindowBehaviorOpenSearchItem);
	newWindowBehaviorMenu->AddItem(fNewWindowBehaviorOpenBlankItem);
	fNewWindowBehaviorMenu = new BMenuField("new window behavior",
		B_TRANSLATE("New windows:"), newWindowBehaviorMenu);

	BPopUpMenu* newTabBehaviorMenu = new BPopUpMenu("New tabs");
	newTabBehaviorMenu->AddItem(fNewTabBehaviorOpenBlankItem);
	newTabBehaviorMenu->AddItem(fNewTabBehaviorOpenHomeItem);
	newTabBehaviorMenu->AddItem(fNewTabBehaviorOpenSearchItem);
	newTabBehaviorMenu->AddItem(fNewTabBehaviorCloneCurrentItem);
	fNewTabBehaviorMenu = new BMenuField("new tab behavior",
		B_TRANSLATE("New tabs:"), newTabBehaviorMenu);

	fDaysInHistory = new BSpinner("days in history",
		B_TRANSLATE("Number of days to keep links in History menu:"),
		new BMessage(MSG_HISTORY_MENU_DAYS_CHANGED));
	fDaysInHistory->SetRange(1, 35);
	fDaysInHistory->SetValue(
		BrowsingHistory::DefaultInstance()->MaxHistoryItemAge());

	fShowTabsIfOnlyOnePage = new BCheckBox("show tabs if only one page",
		B_TRANSLATE("Show tabs if only one page is open"),
		new BMessage(MSG_TAB_DISPLAY_BEHAVIOR_CHANGED));
	fShowTabsIfOnlyOnePage->SetValue(B_CONTROL_ON);

	fAutoHideInterfaceInFullscreenMode = new BCheckBox("auto-hide interface",
		B_TRANSLATE("Auto-hide interface in full screen mode"),
		new BMessage(MSG_AUTO_HIDE_INTERFACE_BEHAVIOR_CHANGED));
	fAutoHideInterfaceInFullscreenMode->SetValue(B_CONTROL_OFF);

	fAutoHidePointer = new BCheckBox("auto-hide pointer",
		B_TRANSLATE("Auto-hide mouse pointer"),
		new BMessage(MSG_AUTO_HIDE_POINTER_BEHAVIOR_CHANGED));
	fAutoHidePointer->SetValue(B_CONTROL_OFF);

	fShowHomeButton = new BCheckBox("show home button",
		B_TRANSLATE("Show home button"),
		new BMessage(MSG_SHOW_HOME_BUTTON_CHANGED));
	fShowHomeButton->SetValue(B_CONTROL_ON);

	fAutoHideTimeout = new BSpinner("auto hide timeout",
		B_TRANSLATE("Auto-hide interface in full screen mode after (seconds):"),
		new BMessage(MSG_AUTO_HIDE_INTERFACE_BEHAVIOR_CHANGED));
	fAutoHideTimeout->SetRange(1, 10);
	fAutoHideTimeout->SetValue(1);

	BView* view = BGroupLayoutBuilder(B_VERTICAL, 0)
		.Add(BGridLayoutBuilder(spacing / 2, spacing / 2)
			.Add(fStartPageControl->CreateLabelLayoutItem(), 0, 0)
			.Add(fStartPageControl->CreateTextViewLayoutItem(), 1, 0, 4)

			.Add(fSearchPageMenu->CreateLabelLayoutItem(), 0, 1)
			.Add(fSearchPageMenu->CreateMenuBarLayoutItem(), 1, 1)

			.Add(fSearchPageControl->CreateLabelLayoutItem(), 2, 1)
			.Add(fSearchPageControl->CreateTextViewLayoutItem(), 3, 1, 2)

			.Add(fStartUpBehaviorMenu->CreateLabelLayoutItem(), 0, 2)
			.Add(fStartUpBehaviorMenu->CreateMenuBarLayoutItem(), 1, 2, 4)

			.Add(fNewWindowBehaviorMenu->CreateLabelLayoutItem(), 0, 3)
			.Add(fNewWindowBehaviorMenu->CreateMenuBarLayoutItem(), 1, 3, 4)

			.Add(fNewTabBehaviorMenu->CreateLabelLayoutItem(), 0, 4)
			.Add(fNewTabBehaviorMenu->CreateMenuBarLayoutItem(), 1, 4, 4)

			.Add(fDownloadFolderControl->CreateLabelLayoutItem(), 0, 5)
			.Add(fDownloadFolderControl->CreateTextViewLayoutItem(), 1, 5, 3)
			.Add(fChooseButton, 4, 5)
		)
		.Add(BSpaceLayoutItem::CreateVerticalStrut(spacing))
		.Add(new BSeparatorView(B_HORIZONTAL, B_PLAIN_BORDER))
		.Add(BSpaceLayoutItem::CreateVerticalStrut(spacing))
		.Add(fShowTabsIfOnlyOnePage)
		.Add(fAutoHideInterfaceInFullscreenMode)
		.Add(fAutoHidePointer)
		.Add(fShowHomeButton)
		.Add(BSpaceLayoutItem::CreateVerticalStrut(spacing))

		.AddGroup(B_HORIZONTAL)
			.Add(fDaysInHistory)
			.AddGlue()
			.End()
		.AddGroup(B_HORIZONTAL)
			.Add(fAutoHideTimeout)
			.AddGlue()
			.End()
		.AddGlue()
		.SetInsets(B_USE_WINDOW_SPACING, B_USE_WINDOW_SPACING,
			B_USE_WINDOW_SPACING, B_USE_DEFAULT_SPACING)
		.TopView()
	;
	view->SetName(B_TRANSLATE("General"));
	return view;
}


BView*
SettingsWindow::_CreateFontsPage(float spacing)
{
	fStandardFontView = new FontSelectionView("standard",
		B_TRANSLATE("Standard font:"), true, be_plain_font);
	BFont defaultSerifFont = _FindDefaultSerifFont();
	fSerifFontView = new FontSelectionView("serif",
		B_TRANSLATE("Serif font:"), true, &defaultSerifFont);
	fSansSerifFontView = new FontSelectionView("sans serif",
		B_TRANSLATE("Sans serif font:"), true, be_plain_font);
	fFixedFontView = new FontSelectionView("fixed",
		B_TRANSLATE("Fixed font:"), true, be_fixed_font);

	fStandardSizesSpinner = new BSpinner("standard font size",
		B_TRANSLATE("Default standard font size:"),
		new BMessage(MSG_STANDARD_FONT_SIZE_SELECTED));
	fStandardSizesSpinner->SetAlignment(B_ALIGN_RIGHT);

	fFixedSizesSpinner = new BSpinner("fixed font size",
		B_TRANSLATE("Default fixed font size:"),
		new BMessage(MSG_FIXED_FONT_SIZE_SELECTED));
	fFixedSizesSpinner->SetAlignment(B_ALIGN_RIGHT);

	BView* view = BGridLayoutBuilder(spacing / 2, spacing / 2)
		.Add(fStandardFontView->CreateFontsLabelLayoutItem(), 0, 0)
		.Add(fStandardFontView->CreateFontsMenuBarLayoutItem(), 1, 0)
		.Add(fStandardSizesSpinner->CreateLabelLayoutItem(), 2, 0)
		.Add(fStandardSizesSpinner->CreateTextViewLayoutItem(), 3, 0)
		.Add(fStandardFontView->PreviewBox(), 1, 1, 3)
		.Add(fSerifFontView->CreateFontsLabelLayoutItem(), 0, 2)
		.Add(fSerifFontView->CreateFontsMenuBarLayoutItem(), 1, 2)
		.Add(fSerifFontView->PreviewBox(), 1, 3, 3)
		.Add(fSansSerifFontView->CreateFontsLabelLayoutItem(), 0, 4)
		.Add(fSansSerifFontView->CreateFontsMenuBarLayoutItem(), 1, 4)
		.Add(fSansSerifFontView->PreviewBox(), 1, 5, 3)
		.Add(BSpaceLayoutItem::CreateVerticalStrut(spacing / 2), 0, 6, 2)
		.Add(fFixedFontView->CreateFontsLabelLayoutItem(), 0, 7)
		.Add(fFixedFontView->CreateFontsMenuBarLayoutItem(), 1, 7)
		.Add(fFixedSizesSpinner->CreateLabelLayoutItem(), 2, 7)
		.Add(fFixedSizesSpinner->CreateTextViewLayoutItem(), 3, 7)
		.Add(fFixedFontView->PreviewBox(), 1, 8, 3)
		.SetInsets(B_USE_WINDOW_SPACING, B_USE_WINDOW_SPACING,
			B_USE_WINDOW_SPACING, B_USE_DEFAULT_SPACING)
		.View();

	view->SetName(B_TRANSLATE("Fonts"));
	return view;
}


BView*
SettingsWindow::_CreateProxyPage(float spacing)
{
	fUseProxyCheckBox = new BCheckBox("use proxy",
		B_TRANSLATE("Use proxy server to connect to the internet"),
		new BMessage(MSG_USE_PROXY_CHANGED));
	fUseProxyCheckBox->SetValue(B_CONTROL_ON);

	fProxyAddressControl = new BTextControl("proxy address",
		B_TRANSLATE("Proxy server address:"), "",
		new BMessage(MSG_PROXY_ADDRESS_CHANGED));
	fProxyAddressControl->SetModificationMessage(
		new BMessage(MSG_PROXY_ADDRESS_CHANGED));
	fProxyAddressControl->SetText(
		fSettings->GetValue(kSettingsKeyProxyAddress, ""));

	fProxyPortControl = new BTextControl("proxy port",
		B_TRANSLATE("Proxy server port:"), "",
		new BMessage(MSG_PROXY_PORT_CHANGED));
	fProxyPortControl->SetModificationMessage(
		new BMessage(MSG_PROXY_PORT_CHANGED));
	fProxyPortControl->SetText(
		fSettings->GetValue(kSettingsKeyProxyPort, ""));

	fUseProxyAuthCheckBox = new BCheckBox("use authentication",
		B_TRANSLATE("Proxy server requires authentication"),
		new BMessage(MSG_USE_PROXY_AUTH_CHANGED));
	fUseProxyAuthCheckBox->SetValue(B_CONTROL_ON);

	fProxyUsernameControl = new BTextControl("proxy username",
		B_TRANSLATE("Proxy username:"), "",
		new BMessage(MSG_PROXY_USERNAME_CHANGED));
	fProxyUsernameControl->SetModificationMessage(
		new BMessage(MSG_PROXY_USERNAME_CHANGED));
	fProxyUsernameControl->SetText(
		fSettings->GetValue(kSettingsKeyProxyUsername, ""));

	fProxyPasswordControl = new BTextControl("proxy password",
		B_TRANSLATE("Proxy password:"), "",
		new BMessage(MSG_PROXY_PASSWORD_CHANGED));
	fProxyPasswordControl->SetModificationMessage(
		new BMessage(MSG_PROXY_PASSWORD_CHANGED));
	fProxyPasswordControl->TextView()->HideTyping(true);
	fProxyPasswordControl->SetText(
		fSettings->GetValue(kSettingsKeyProxyPassword, ""));

	BView* view = BGridLayoutBuilder(spacing / 2, spacing / 2)
		.Add(fUseProxyCheckBox, 0, 0, 2)
		.Add(fProxyAddressControl->CreateLabelLayoutItem(), 0, 1)
		.Add(fProxyAddressControl->CreateTextViewLayoutItem(), 1, 1, 2)
		.Add(fProxyPortControl->CreateLabelLayoutItem(), 0, 2)
		.Add(fProxyPortControl->CreateTextViewLayoutItem(), 1, 2, 2)
		.Add(BSpaceLayoutItem::CreateVerticalStrut(spacing), 0, 3)
		.Add(fUseProxyAuthCheckBox, 0, 4, 2)
		.Add(fProxyUsernameControl->CreateLabelLayoutItem(), 0, 5)
		.Add(fProxyUsernameControl->CreateTextViewLayoutItem(), 1, 5, 2)
		.Add(fProxyPasswordControl->CreateLabelLayoutItem(), 0, 6)
		.Add(fProxyPasswordControl->CreateTextViewLayoutItem(), 1, 6, 2)
		.Add(BSpaceLayoutItem::CreateGlue(), 0, 7)
		.SetInsets(B_USE_WINDOW_SPACING, B_USE_WINDOW_SPACING,
			B_USE_WINDOW_SPACING, B_USE_DEFAULT_SPACING)
		.View();

	view->SetName(B_TRANSLATE("Proxy server"));
	return view;
}


void
SettingsWindow::_SetupFontSelectionView(FontSelectionView* view,
	BMessage* message)
{
	AddHandler(view);
	view->AttachedToLooper();
	view->SetMessage(message);
	view->SetTarget(this);
}


// #pragma mark -


bool
SettingsWindow::_CanApplySettings() const
{
	for (int32 i = 0; i < fSettingsCount; i++) {
		const Setting& setting = fSettingsData[i];
		if (setting.control == NULL)
			continue;
		switch (setting.type) {
			case B_STRING_TYPE:
			{
				BTextControl* textControl
					= dynamic_cast<BTextControl*>(setting.control);
				if (textControl && textControl->Text() != fSettings->GetValue(setting.key,
						setting.defaultValue.ToString())) {
					return true;
				}
				break;
			}
			case B_BOOL_TYPE:
			{
				BCheckBox* checkBox
					= dynamic_cast<BCheckBox*>(setting.control);
				if (checkBox && (checkBox->Value() == B_CONTROL_ON) != fSettings->GetValue(
						setting.key, setting.defaultValue.ToBool())) {
					return true;
				}
				break;
			}
			default:
				break;
		}
	}

	if (fDaysInHistory->Value()
		!= BrowsingHistory::DefaultInstance()->MaxHistoryItemAge())
		return true;

	if (_StartUpPolicy()
		!= fSettings->GetValue(kSettingsKeyStartUpPolicy,
			(uint32)ResumePriorSession))
		return true;

	if (_NewWindowPolicy()
		!= fSettings->GetValue(kSettingsKeyNewWindowPolicy,
			(uint32)OpenStartPage))
		return true;

	if (_NewTabPolicy()
		!= fSettings->GetValue(kSettingsKeyNewTabPolicy,
			(uint32)OpenBlankPage))
		return true;

	if (fStandardFontView->Font()
		!= fSettings->GetValue("standard font", *be_plain_font))
		return true;

	if (fSerifFontView->Font()
		!= fSettings->GetValue("serif font", _FindDefaultSerifFont()))
		return true;

	if (fSansSerifFontView->Font()
		!= fSettings->GetValue("sans serif font", *be_plain_font))
		return true;

	if (fFixedFontView->Font()
		!= fSettings->GetValue("fixed font", *be_fixed_font))
		return true;

	if (fStandardSizesSpinner->Value()
		!= fSettings->GetValue("standard font size", kDefaultFontSize))
		return true;

	if (fFixedSizesSpinner->Value()
		!= fSettings->GetValue("fixed font size", kDefaultFontSize))
		return true;

	if ((fUseProxyCheckBox->Value() == B_CONTROL_ON)
		!= fSettings->GetValue(kSettingsKeyUseProxy, false))
		return true;

	if (strcmp(fProxyAddressControl->Text(),
		fSettings->GetValue(kSettingsKeyProxyAddress, "")) != 0)
		return true;

	if (_ProxyPort()
		!= fSettings->GetValue(kSettingsKeyProxyPort, (uint32)0))
		return true;

	if ((fUseProxyAuthCheckBox->Value() == B_CONTROL_ON)
		!= fSettings->GetValue(kSettingsKeyUseProxyAuth, false))
		return true;

	if (strcmp(fProxyUsernameControl->Text(),
		fSettings->GetValue(kSettingsKeyProxyUsername, "")) != 0)
		return true;

	if (strcmp(fProxyPasswordControl->Text(),
		fSettings->GetValue(kSettingsKeyProxyPassword, "")) != 0)
		return true;

	return false;
}


void
SettingsWindow::_ApplySettings()
{
	for (int32 i = 0; i < fSettingsCount; i++) {
		const Setting& setting = fSettingsData[i];
		switch (setting.type) {
			case B_STRING_TYPE:
			{
				BTextControl* textControl
					= dynamic_cast<BTextControl*>(setting.control);
				if (textControl)
					fSettings->SetValue(setting.key, textControl->Text());
				break;
			}
			case B_BOOL_TYPE:
			{
				BCheckBox* checkBox
					= dynamic_cast<BCheckBox*>(setting.control);
				if (checkBox)
					fSettings->SetValue(setting.key, checkBox->Value() == B_CONTROL_ON);
				break;
			}
			default:
				break;
		}
	}
#if 0
	// Store general settings
	BrowsingHistory::DefaultInstance()->SetMaxHistoryItemAge(
		(uint32)fDaysInHistory->Value());
	fSettings->SetValue(kSettingsKeyStartPageURL, fStartPageControl->Text());
	fSettings->SetValue(kSettingsKeySearchPageURL, fSearchPageControl->Text());
	fSettings->SetValue(kSettingsKeyDownloadPath, fDownloadFolderControl->Text());
	fSettings->SetValue(kSettingsKeyShowTabsIfSinglePageOpen,
		fShowTabsIfOnlyOnePage->Value() == B_CONTROL_ON);
	fSettings->SetValue(kSettingsKeyAutoHideInterfaceInFullscreenMode,
		fAutoHideInterfaceInFullscreenMode->Value() == B_CONTROL_ON);
	fSettings->SetValue(kSettingsKeyAutoHidePointer,
		fAutoHidePointer->Value() == B_CONTROL_ON);
	fSettings->SetValue(kSettingsKeyShowHomeButton,
		fShowHomeButton->Value() == B_CONTROL_ON);

	fSettings->SetValue("auto_hide_timeout", fAutoHideTimeout->Value() * 1000000);

	// New page policies
	fSettings->SetValue(kSettingsKeyStartUpPolicy, _StartUpPolicy());
	fSettings->SetValue(kSettingsKeyNewWindowPolicy, _NewWindowPolicy());
	fSettings->SetValue(kSettingsKeyNewTabPolicy, _NewTabPolicy());

	// Store font settings
	fSettings->SetValue("standard font", fStandardFontView->Font());
	fSettings->SetValue("serif font", fSerifFontView->Font());
	fSettings->SetValue("sans serif font", fSansSerifFontView->Font());
	fSettings->SetValue("fixed font", fFixedFontView->Font());
	int32 standardFontSize = fStandardSizesSpinner->Value();
	int32 fixedFontSize = fFixedSizesSpinner->Value();
	fSettings->SetValue("standard font size", standardFontSize);
	fSettings->SetValue("fixed font size", fixedFontSize);

	// Store proxy settings

	fSettings->SetValue(kSettingsKeyUseProxy,
		fUseProxyCheckBox->Value() == B_CONTROL_ON);
	fSettings->SetValue(kSettingsKeyProxyAddress,
		fProxyAddressControl->Text());
	uint32 proxyPort = _ProxyPort();
	fSettings->SetValue(kSettingsKeyProxyPort, proxyPort);
	fSettings->SetValue(kSettingsKeyUseProxyAuth,
		fUseProxyAuthCheckBox->Value() == B_CONTROL_ON);
	fSettings->SetValue(kSettingsKeyProxyUsername,
		fProxyUsernameControl->Text());
	fSettings->SetValue(kSettingsKeyProxyPassword,
		fProxyPasswordControl->Text());
#endif
	fSettings->Save();

	// Apply settings to default web page settings.
	BWebSettings::Default()->SetStandardFont(fStandardFontView->Font());
	BWebSettings::Default()->SetSerifFont(fSerifFontView->Font());
	BWebSettings::Default()->SetSansSerifFont(fSansSerifFontView->Font());
	BWebSettings::Default()->SetFixedFont(fFixedFontView->Font());
	BWebSettings::Default()->SetDefaultStandardFontSize(fStandardSizesSpinner->Value());
	BWebSettings::Default()->SetDefaultFixedFontSize(fFixedSizesSpinner->Value());

	if (fUseProxyCheckBox->Value() == B_CONTROL_ON) {
		if (fUseProxyAuthCheckBox->Value() == B_CONTROL_ON) {
			BWebSettings::Default()->SetProxyInfo(fProxyAddressControl->Text(),
				_ProxyPort(), B_PROXY_TYPE_HTTP, fProxyUsernameControl->Text(),
				fProxyPasswordControl->Text());
		} else {
			BWebSettings::Default()->SetProxyInfo(fProxyAddressControl->Text(),
				_ProxyPort(), B_PROXY_TYPE_HTTP, "", "");
		}
	} else
		BWebSettings::Default()->SetProxyInfo();

	// This will find all currently instantiated page settings and apply
	// the default values, unless the page settings have local overrides.
	BWebSettings::Default()->Apply();

	_ValidateControlsEnabledStatus();
}


void
SettingsWindow::_RevertSettings()
{
	for (int32 i = 0; i < fSettingsCount; i++) {
		const Setting& setting = fSettingsData[i];
		switch (setting.type) {
			case B_STRING_TYPE:
			{
				BTextControl* textControl
					= dynamic_cast<BTextControl*>(setting.control);
				if (textControl) {
					textControl->SetText(fSettings->GetValue(setting.key,
						setting.defaultValue.ToString()));
				}
				break;
			}
			case B_BOOL_TYPE:
			{
				BCheckBox* checkBox
					= dynamic_cast<BCheckBox*>(setting.control);
				if (checkBox) {
					checkBox->SetValue(fSettings->GetValue(setting.key,
						setting.defaultValue.ToBool()));
				}
				break;
			}
			default:
				break;
		}
	}
#if 0
	fStartPageControl->SetText(
		fSettings->GetValue(kSettingsKeyStartPageURL, kDefaultStartPageURL));

	BString searchPage = fSettings->GetValue(kSettingsKeySearchPageURL,
		kDefaultSearchPageURL);
	fSearchPageControl->SetText(searchPage);

	bool found = false;
	BMenu* searchMenu = fSearchPageMenu->Menu();
	int32 itemCount = searchMenu->CountItems() - 2;
		// Ignore the two last items: separator and "custom"
	for (int i = 0; i < itemCount; i++) {
		BMenuItem* item = searchMenu->ItemAt(i);
		BMessage* message = item->Message();
		if (message->FindString("searchstring") == searchPage) {
			item->SetMarked(true);
			fSearchPageControl->SetEnabled(false);
			found = true;
			break;
		}
	}

	if (!found)
		searchMenu->ItemAt(searchMenu->CountItems() - 1)->SetMarked(true);

	fDownloadFolderControl->SetText(
		fSettings->GetValue(kSettingsKeyDownloadPath, kDefaultDownloadPath));
	fShowTabsIfOnlyOnePage->SetValue(
		fSettings->GetValue(kSettingsKeyShowTabsIfSinglePageOpen, true));
	fAutoHideInterfaceInFullscreenMode->SetValue(
		fSettings->GetValue(kSettingsKeyAutoHideInterfaceInFullscreenMode,
			false));
	fAutoHidePointer->SetValue(
		fSettings->GetValue(kSettingsKeyAutoHidePointer, false));
	fShowHomeButton->SetValue(
		fSettings->GetValue(kSettingsKeyShowHomeButton, true));

	fAutoHideTimeout->SetValue(
		fSettings->GetValue("auto_hide_timeout", 1000000) / 1000000);

	fDaysInHistory->SetValue(
		BrowsingHistory::DefaultInstance()->MaxHistoryItemAge());

	// Start Up policy
	uint32 startUpPolicy = fSettings->GetValue(kSettingsKeyStartUpPolicy,
		(uint32)ResumePriorSession);
	switch (startUpPolicy) {
		default:
		case ResumePriorSession:
			fStartUpBehaviorResumePriorSession->SetMarked(true);
			break;
		case StartNewSession:
			fStartUpBehaviorStartNewSession->SetMarked(true);
			break;
	}

	// New window policy
	uint32 newWindowPolicy = fSettings->GetValue(kSettingsKeyNewWindowPolicy,
		(uint32)OpenStartPage);
	switch (newWindowPolicy) {
		default:
		case OpenStartPage:
			fNewWindowBehaviorOpenHomeItem->SetMarked(true);
			break;
		case OpenSearchPage:
			fNewWindowBehaviorOpenSearchItem->SetMarked(true);
			break;
		case OpenBlankPage:
			fNewWindowBehaviorOpenBlankItem->SetMarked(true);
			break;
	}

	// New tab policy
	uint32 newTabPolicy = fSettings->GetValue(kSettingsKeyNewTabPolicy,
		(uint32)OpenBlankPage);
	switch (newTabPolicy) {
		default:
		case OpenBlankPage:
			fNewTabBehaviorOpenBlankItem->SetMarked(true);
			break;
		case OpenStartPage:
			fNewTabBehaviorOpenHomeItem->SetMarked(true);
			break;
		case OpenSearchPage:
			fNewTabBehaviorOpenSearchItem->SetMarked(true);
			break;
		case CloneCurrentPage:
			fNewTabBehaviorCloneCurrentItem->SetMarked(true);
			break;
	}

	// Font settings
	int32 defaultFontSize = fSettings->GetValue("standard font size",
		kDefaultFontSize);
	int32 defaultFixedFontSize = fSettings->GetValue("fixed font size",
		kDefaultFontSize);

	fStandardSizesSpinner->SetValue(defaultFontSize);
	fFixedSizesSpinner->SetValue(defaultFixedFontSize);

	fStandardFontView->SetFont(fSettings->GetValue("standard font",
		*be_plain_font), defaultFontSize);
	fSerifFontView->SetFont(fSettings->GetValue("serif font",
		_FindDefaultSerifFont()), defaultFontSize);
	fSansSerifFontView->SetFont(fSettings->GetValue("sans serif font",
		*be_plain_font), defaultFontSize);
	fFixedFontView->SetFont(fSettings->GetValue("fixed font",
		*be_fixed_font), defaultFixedFontSize);

	// Proxy settings
	fUseProxyCheckBox->SetValue(fSettings->GetValue(kSettingsKeyUseProxy,
		false));
	fProxyAddressControl->SetText(fSettings->GetValue(kSettingsKeyProxyAddress,
		""));
	BString keyProxyPort;
	keyProxyPort << fSettings->GetValue(kSettingsKeyProxyPort, (uint32)0);
	fProxyPortControl->SetText(keyProxyPort.String());
	fUseProxyAuthCheckBox->SetValue(fSettings->GetValue(kSettingsKeyUseProxyAuth,
		false));
	fProxyUsernameControl->SetText(fSettings->GetValue(kSettingsKeyProxyUsername,
		""));
	fProxyPasswordControl->SetText(fSettings->GetValue(kSettingsKeyProxyPassword,
		""));
#endif
	_ValidateProxyPort();
	_ValidateStartPage();
	_ValidateSearchPage();
	_ValidateProxyAddress();
	_ValidateControlsEnabledStatus();
}


void
SettingsWindow::_ChooseDownloadFolder(const BMessage* message)
{
	if (fOpenFilePanel == NULL) {
		BMessenger target(this);
		fOpenFilePanel = new (std::nothrow) BFilePanel(B_OPEN_PANEL,
			&target, NULL, B_DIRECTORY_NODE);
	}
	BMessage panelMessage(MSG_HANDLE_DOWNLOAD_FOLDER);
	fOpenFilePanel->SetMessage(&panelMessage);
	fOpenFilePanel->Show();
}


void
SettingsWindow:: _HandleDownloadPanelResult(BFilePanel* panel,
	const BMessage* message)
{
	entry_ref ref;
	if (message->FindRef("refs", 0, &ref) == B_OK)
	{
		BPath path(&ref);
		fDownloadFolderControl->SetText(path.Path());
	}
}


void
SettingsWindow::_ValidateControlsEnabledStatus()
{
	bool canApply = _CanApplySettings();
	fApplyButton->SetEnabled(canApply && fProxyPortValid && fStartPageValid && fSearchPageValid && fProxyAddressValid);
	fRevertButton->SetEnabled(canApply);
	// Let the Cancel button be enabled always, as another way to close the
	// window...
	fCancelButton->SetEnabled(true);

	bool useProxy = fUseProxyCheckBox->Value() == B_CONTROL_ON;
	fProxyAddressControl->SetEnabled(useProxy);
	fProxyPortControl->SetEnabled(useProxy);
	fUseProxyAuthCheckBox->SetEnabled(useProxy);
	bool useProxyAuth = useProxy && fUseProxyAuthCheckBox->Value() == B_CONTROL_ON;
	fProxyUsernameControl->SetEnabled(useProxyAuth);
	fProxyPasswordControl->SetEnabled(useProxyAuth);
}


// #pragma mark -


void
SettingsWindow::_ValidateProxyAddress()
{
	BUrl url(fProxyAddressControl->Text(), true);
	if (url.Host().Length() == 0) {
		fProxyAddressControl->TextView()->SetViewColor(255, 200, 200);
		fProxyAddressValid = false;
	} else {
		fProxyAddressControl->TextView()->SetViewUIColor(B_DOCUMENT_BACKGROUND_COLOR);
		fProxyAddressValid = true;
	}
	fProxyAddressControl->TextView()->Invalidate();
}


void
SettingsWindow::_ValidateSearchPage()
{
	BString url(fSearchPageControl->Text());
	if (url.FindFirst("%s") < 0) {
		fSearchPageControl->TextView()->SetViewColor(255, 200, 200);
		fSearchPageValid = false;
	} else {
		fSearchPageControl->TextView()->SetViewUIColor(B_DOCUMENT_BACKGROUND_COLOR);
		fSearchPageValid = true;
	}
	fSearchPageControl->TextView()->Invalidate();
}


void
SettingsWindow::_ValidateStartPage()
{
	BUrl url(fStartPageControl->Text(), true);
	if (!url.IsValid()) {
		fStartPageControl->TextView()->SetViewColor(255, 200, 200);
		fStartPageValid = false;
	} else {
		fStartPageControl->TextView()->SetViewUIColor(B_DOCUMENT_BACKGROUND_COLOR);
		fStartPageValid = true;
	}
	fStartPageControl->TextView()->Invalidate();
}


void
SettingsWindow::_ValidateProxyPort()
{
	const char* text = fProxyPortControl->Text();
	char* end;
	errno = 0;
	long port = strtoul(text, &end, 10);

	if (errno != 0 || *end != '\0' || port < 0 || port > 65535) {
		// Invalid input
		fProxyPortControl->TextView()->SetViewColor(255, 200, 200);
		fProxyPortValid = false;
	} else {
		// Valid input
		fProxyPortControl->TextView()->SetViewUIColor(B_DOCUMENT_BACKGROUND_COLOR);
		fProxyPortValid = true;
	}
	fProxyPortControl->TextView()->Invalidate();
}


uint32
SettingsWindow::_StartUpPolicy() const
{
	uint32 startUpPolicy = ResumePriorSession;
	BMenuItem* markedItem = fStartUpBehaviorMenu->Menu()->FindMarked();
	if (markedItem == fStartUpBehaviorStartNewSession)
		startUpPolicy = StartNewSession;
	return startUpPolicy;
}

uint32
SettingsWindow::_NewWindowPolicy() const
{
	uint32 newWindowPolicy = OpenStartPage;
	BMenuItem* markedItem = fNewWindowBehaviorMenu->Menu()->FindMarked();
	if (markedItem == fNewWindowBehaviorOpenSearchItem)
		newWindowPolicy = OpenSearchPage;
	else if (markedItem == fNewWindowBehaviorOpenBlankItem)
		newWindowPolicy = OpenBlankPage;
	return newWindowPolicy;
}


uint32
SettingsWindow::_NewTabPolicy() const
{
	uint32 newTabPolicy = OpenBlankPage;
	BMenuItem* markedItem = fNewTabBehaviorMenu->Menu()->FindMarked();
	if (markedItem == fNewTabBehaviorCloneCurrentItem)
		newTabPolicy = CloneCurrentPage;
	else if (markedItem == fNewTabBehaviorOpenHomeItem)
		newTabPolicy = OpenStartPage;
	else if (markedItem == fNewTabBehaviorOpenSearchItem)
		newTabPolicy = OpenSearchPage;
	return newTabPolicy;
}


BFont
SettingsWindow::_FindDefaultSerifFont() const
{
	// Default to the first "serif" font we find.
	BFont serifFont(*be_plain_font);
	font_family family;
	int32 familyCount = count_font_families();
	for (int32 i = 0; i < familyCount; i++) {
		if (get_font_family(i, &family) == B_OK) {
			BString familyString(family);
			if (familyString.IFindFirst("sans") >= 0)
				continue;
			if (familyString.IFindFirst("serif") >= 0) {
				serifFont.SetFamilyAndFace(family, B_REGULAR_FACE);
				break;
			}
		}
	}
	return serifFont;
}


uint32
SettingsWindow::_ProxyPort() const
{
	const char* text = fProxyPortControl->Text();
	char* end;
	long port = strtoul(text, &end, 10);
	if (*end != '\0' || port < 0 || port > 65535)
		return 0;
	return (uint32)port;
}
