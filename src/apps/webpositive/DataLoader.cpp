/*
 * Copyright 2025, Your Name <your@email.com>
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "DataLoader.h"

#include <Application.h>
#include <Message.h>

#include "BrowserApp.h"
#include "BookmarkManager.h"
#include "BrowsingHistory.h"
#include "WebViewConstants.h"


enum {
	MSG_DATA_LOADED = 'dtld'
};


DataLoader::DataLoader(BHandler* target)
	:
	BLooper("DataLoader"),
	fTarget(target)
{
	Run();
}


void
DataLoader::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case MSG_LOAD_HISTORY:
		{
			// Load the browsing history.
			BObjectList<BrowsingHistoryItem, true>* historyItems
				= new BObjectList<BrowsingHistoryItem, true>(20, true);
			BrowsingHistory* history = BrowsingHistory::DefaultInstance();
			if (history->Lock()) {
				for (int32 i = 0; i < history->CountItems(); i++) {
					historyItems->AddItem(
						new BrowsingHistoryItem(history->HistoryItemAt(i)));
				}
				history->Unlock();
			}

			// Create a message with the loaded data.
			BMessage reply(MSG_HISTORY_LOADED);
			reply.AddPointer("history", historyItems);

			// Send the message to the target.
			fTarget->Looper()->PostMessage(&reply, fTarget);
			break;
		}

		case MSG_LOAD_BOOKMARKS:
		{
			BookmarkManager* bookmarkManager;
			if (message->FindPointer("manager",
					(void**)&bookmarkManager) != B_OK)
				break;
			bookmarkManager->_LoadBookmarkURLs();
			break;
		}

		case MSG_CREATE_BOOKMARK:
		{
			BookmarkManager* bookmarkManager;
			if (message->FindPointer("manager",
					(void**)&bookmarkManager) != B_OK)
				break;
			BMessage data;
			if (message->FindMessage("data", &data) != B_OK)
				break;
			bookmarkManager->CreateBookmark(&data);
			break;
		}

		default:
			BLooper::MessageReceived(message);
	}
}
