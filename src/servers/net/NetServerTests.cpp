/*
 * Copyright 2025, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include <stdio.h>

#include <NetServer.h>
#include <DHCPClient.h>
#include <AutoconfigLooper.h>

#include "Services.h"


void
TestNetServerMemoryLeaks()
{
	printf("Testing NetServer memory leaks... ");

	status_t error;
	NetServer* server = new NetServer(error);
	server->Run();

	BMessage* suggestedInterface = new BMessage();
	BStringList devicesAlreadyConfigured;
	server->_ConfigureDevices("/dev/net", devicesAlreadyConfigured,
		suggestedInterface);

	BMessage joinMessage(kMsgJoinNetwork);
	joinMessage.AddString("device", "loop");
	server->_JoinNetwork(joinMessage);

	delete server;

	printf("OK\n");
}


void
TestDHCPClientMemoryLeaks()
{
	printf("Testing DHCPClient memory leaks... ");

	BMessenger target;
	DHCPClient* client = new DHCPClient(target, "loop");
	client->Initialize();
	delete client;

	printf("OK\n");
}


int
main(int argc, char** argv)
{
	TestNetServerMemoryLeaks();
	TestDHCPClientMemoryLeaks();

	return 0;
}
