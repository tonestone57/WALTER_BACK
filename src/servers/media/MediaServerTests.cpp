/*
 * Copyright 2025, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include <stdio.h>

#include <media_server.h>
#include <NodeManager.h>
#include <BufferManager.h>
#include <AppManager.h>

#include "ServerInterface.h"


void
TestMediaServerMemoryLeaks()
{
	printf("Testing media_server memory leaks... ");

	status_t error;
	ServerApp* server = new ServerApp(error);
	server->Run();

	server->_LaunchAddOnServer();

	delete server;

	printf("OK\n");
}


void
TestNodeManagerMemoryLeaks()
{
	printf("Testing NodeManager memory leaks... ");

	NodeManager* nodeManager = new NodeManager();
	dormant_node_info infos[1];
	int32 count = 1;
	nodeManager->GetDormantNodes(infos, &count, NULL, NULL, NULL, 0, 0);
	delete nodeManager;

	printf("OK\n");
}


void
TestBufferManagerMemoryLeaks()
{
	printf("Testing BufferManager memory leaks... ");

	BufferManager* bufferManager = new BufferManager();
	media_buffer_id bufferID;
	bufferManager->RegisterBuffer(0, 1024, 0, 0, 0, &bufferID);
	delete bufferManager;

	printf("OK\n");
}


void
TestAppManagerRaceCondition()
{
	printf("Testing AppManager race condition... ");

	AppManager* appManager = new AppManager();
	appManager->RegisterTeam(0, BMessenger());
	appManager->UnregisterTeam(0);
	delete appManager;

	printf("OK\n");
}


int
main(int argc, char** argv)
{
	TestMediaServerMemoryLeaks();
	TestNodeManagerMemoryLeaks();
	TestBufferManagerMemoryLeaks();
	TestAppManagerRaceCondition();

	return 0;
}
