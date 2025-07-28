/*
 * Copyright 2025, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include <stdio.h>

#include <Registrar.h>
#include <TRoster.h>
#include <MIMEManager.h>
#include <ClipboardHandler.h>

#include "Commands.h"


void
TestRegistrarMemoryLeaks()
{
	printf("Testing Registrar memory leaks... ");

	status_t error;
	Registrar* registrar = new Registrar(&error);
	BServer* server = dynamic_cast<BServer*>(registrar);
	server->Run();

	delete registrar;

	printf("OK\n");
}


void
TestTRosterMemoryLeaks()
{
	printf("Testing TRoster memory leaks... ");

	TRoster* roster = new TRoster();
	roster->HandleLoadRecentLists(NULL);
	roster->HandleRestartAppServer(new BMessage(kMsgRestartAppServer));
	delete roster;

	printf("OK\n");
}


void
TestMIMEManagerMemoryLeaks()
{
	printf("Testing MIMEManager memory leaks... ");

	MIMEManager* mimeManager = new MIMEManager();
	BMessage* message = new BMessage(B_REG_MIME_SET_PARAM);
	message->AddString("type", "application/octet-stream");
	message->AddInt32("which", B_REG_MIME_ICON);
	BBitmap* bitmap = new BBitmap(BRect(0, 0, 15, 15), B_CMAP8);
	message->AddData("icon data", B_RAW_TYPE, bitmap, sizeof(BBitmap));
	delete bitmap;
	message->AddInt32("icon size", 16);
	mimeManager->MessageReceived(message);
	delete message;
	delete mimeManager;

	printf("OK\n");
}


void
TestClipboardHandlerMemoryLeaks()
{
	printf("Testing ClipboardHandler memory leaks... ");

	ClipboardHandler* clipboardHandler = new ClipboardHandler();
	BMessage* message = new BMessage(B_REG_ADD_CLIPBOARD);
	message->AddString("name", "test_clipboard");
	clipboardHandler->MessageReceived(message);
	delete message;
	delete clipboardHandler;

	printf("OK\n");
}


int
main(int argc, char** argv)
{
	TestRegistrarMemoryLeaks();
	TestTRosterMemoryLeaks();
	TestMIMEManagerMemoryLeaks();
	TestClipboardHandlerMemoryLeaks();

	return 0;
}
