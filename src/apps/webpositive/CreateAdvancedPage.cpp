BView*
SettingsWindow::_CreateAdvancedPage(float spacing)
{
	fMaxConnectionsSpinner = new BSpinner("max connections",
		B_TRANSLATE("Maximum connections per host:"),
		new BMessage(MSG_MAX_CONNECTIONS_CHANGED));
	fMaxConnectionsSpinner->SetRange(1, 16);
	fMaxConnectionsSpinner->SetValue(
		fSettings->GetValue(kSettingsKeyMaxConnections, 6));

	BView* view = BGridLayoutBuilder(spacing / 2, spacing / 2)
		.Add(fMaxConnectionsSpinner->CreateLabelLayoutItem(), 0, 0)
		.Add(fMaxConnectionsSpinner->CreateTextViewLayoutItem(), 1, 0)
		.AddGlue(2, 0)
		.Add(BSpaceLayoutItem::CreateGlue(), 0, 1, 4)
		.SetInsets(B_USE_WINDOW_SPACING, B_USE_WINDOW_SPACING,
			B_USE_WINDOW_SPACING, B_USE_DEFAULT_SPACING)
		.View();

	view->SetName(B_TRANSLATE("Advanced"));
	return view;
}
