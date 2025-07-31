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
	// This is where the actual data loading happens.
	// For now, we'll just simulate a delay.
	snooze(2000000); // 2 seconds

	// Create a message with the loaded data.
	BMessage* message = new BMessage(MSG_DATA_LOADED);
	// TODO: Add the actual loaded data to the message.

	// Send the message to the target.
	fTarget.SendMessage(message);

	return 0;
}
