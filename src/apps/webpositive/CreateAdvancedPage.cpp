BView*
SettingsWindow::_CreateAdvancedPage(float spacing)
{
	fEnableTabDiscarding = new BCheckBox("enable tab discarding",
		B_TRANSLATE("Discard tabs to save memory"),
		new BMessage(MSG_ENABLE_TAB_DISCARDING_CHANGED));
	fEnableTabDiscarding->SetValue(
		fSettings->GetValue(kSettingsKeyEnableTabDiscarding, false));

	fMaxActiveTabsSpinner = new BSpinner("max active tabs",
		B_TRANSLATE("Maximum active tabs:"),
		new BMessage(MSG_MAX_ACTIVE_TABS_CHANGED));
	fMaxActiveTabsSpinner->SetRange(1, 100);
	fMaxActiveTabsSpinner->SetValue(
		fSettings->GetValue(kSettingsKeyMaxActiveTabs, 10));

	fMaxConnectionsSpinner = new BSpinner("max connections",
		B_TRANSLATE("Maximum connections per host:"),
		new BMessage(MSG_MAX_CONNECTIONS_CHANGED));
	fMaxConnectionsSpinner->SetRange(1, 16);
	fMaxConnectionsSpinner->SetValue(
		fSettings->GetValue(kSettingsKeyMaxConnections, 6));

	BView* view = BGridLayoutBuilder(spacing / 2, spacing / 2)
		.Add(fEnableTabDiscarding, 0, 0, 2)
		.Add(fMaxActiveTabsSpinner->CreateLabelLayoutItem(), 0, 1)
		.Add(fMaxActiveTabsSpinner->CreateTextViewLayoutItem(), 1, 1)
		.AddGlue(2, 1)
		.Add(fMaxConnectionsSpinner->CreateLabelLayoutItem(), 0, 2)
		.Add(fMaxConnectionsSpinner->CreateTextViewLayoutItem(), 1, 2)
		.AddGlue(2, 2)
		.Add(BSpaceLayoutItem::CreateGlue(), 0, 3, 4)
		.SetInsets(B_USE_WINDOW_SPACING, B_USE_WINDOW_SPACING,
			B_USE_WINDOW_SPACING, B_USE_DEFAULT_SPACING)
		.View();

	view->SetName(B_TRANSLATE("Advanced"));
	return view;
}
