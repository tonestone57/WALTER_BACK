/*
 * Copyright 2025, Your Name <your@email.com>
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "DataLoader.h"

#include <Application.h>
#include <Message.h>

#include "BrowserApp.h"
#include "BrowsingHistory.h"


enum {
	MSG_DATA_LOADED = 'dtld'
};


DataLoader::DataLoader(BMessenger target)
	:
	fThread(-1),
	fTarget(target)
{
}


DataLoader::~DataLoader()
{
	if (fThread >= 0)
		wait_for_thread(fThread, NULL);
}


void
DataLoader::Start()
{
	fThread = spawn_thread(_ThreadFunction, "data loader", B_NORMAL_PRIORITY,
		this);
	if (fThread >= 0)
		resume_thread(fThread);
}


int32
DataLoader::_ThreadFunction(void* data)
{
	return static_cast<DataLoader*>(data)->_LoadData();
}


int32
DataLoader::_LoadData()
{
	// Load the browsing history.
	BObjectList<BrowsingHistoryItem, true>* historyItems = new BObjectList<BrowsingHistoryItem, true>(20);
	BrowsingHistory* history = BrowsingHistory::DefaultInstance();
	if (history->Lock()) {
		for (int32 i = 0; i < history->CountItems(); i++)
			historyItems->AddItem(new BrowsingHistoryItem(history->HistoryItemAt(i)));
		history->Unlock();
	}

	// Create a message with the loaded data.
	BMessage message(MSG_DATA_LOADED);
	message.AddPointer("history", historyItems);

	// Send the message to the target.
	fTarget.SendMessage(&message);

	return 0;
}
