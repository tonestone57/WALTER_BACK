/*
 * Copyright 2025, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include <stdio.h>

#include <InputServer.h>
#include <AddOnManager.h>
#include <InputServerDevice.h>

#include "InputServerTypes.h"


void
TestInputServerMemoryLeaks()
{
	printf("Testing InputServer memory leaks... ");

	InputServer* server = new InputServer();
	server->Run();

	server->_LoadSystemKeymap();

	delete server;

	printf("OK\n");
}


void
TestAddOnManagerMemoryLeaks()
{
	printf("Testing AddOnManager memory leaks... ");

	AddOnManager* addOnManager = new AddOnManager();
	BEntry entry;
	addOnManager->_RegisterDevice(new BInputServerDevice(), entry, 0);
	addOnManager->_RegisterFilter(new BInputServerFilter(), entry, 0);
	addOnManager->_RegisterMethod(new BInputServerMethod(), entry, 0);
	delete addOnManager;

	printf("OK\n");
}


void
TestInputServerDeviceMemoryLeaks()
{
	printf("Testing BInputServerDevice memory leaks... ");

	BInputServerDevice* device = new BInputServerDevice();
	delete device;

	printf("OK\n");
}


int
main(int argc, char** argv)
{
	TestInputServerMemoryLeaks();
	TestAddOnManagerMemoryLeaks();
	TestInputServerDeviceMemoryLeaks();

	return 0;
}
