/*
 * Copyright 2018, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef WEBVIEW_CONSTANTS_H
#define WEBVIEW_CONSTANTS_H

enum {
	// WebPage -> BWebWindow
	B_EDITING_CAPABILITIES_RESULT			= 'ecrs',
	B_PAGE_SOURCE_RESULT					= 'psrs',

	// IconLoader -> BWebWindow
	B_ICON_REQUEST							= 'icrq',
	B_ICON_RESULT							= 'icrs',

	// BrowserWindow -> DataLoader
	MSG_LOAD_HISTORY						= 'ldhs',
	MSG_LOAD_BOOKMARKS						= 'ldbm',
	MSG_CREATE_BOOKMARK						= 'crbm',

	// DataLoader -> BrowserWindow
	MSG_HISTORY_LOADED						= 'hsls',
};

#endif // WEBVIEW_CONSTANTS_H
