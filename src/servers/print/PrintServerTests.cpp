/*
 * Copyright 2025, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include <stdio.h>

#include <PrintServerApp.h>
#include <Printer.h>
#include <Transport.h>

#include "pr_server.h"


void
TestPrintServerAppMemoryLeaks()
{
	printf("Testing PrintServerApp memory leaks... ");

	status_t error;
	PrintServerApp* server = new PrintServerApp(&error);
	server->Run();

	server->CreatePrinter("Test Printer", "Test Driver", "Local",
		"Test Transport", "/dev/null");

	delete server;

	printf("OK\n");
}


void
TestPrinterMemoryLeaks()
{
	printf("Testing Printer memory leaks... ");

	BDirectory dir;
	Printer* printer = new Printer(&dir, NULL);
	Job* job = new Job(NULL, NULL, 0);
	printer->PrintThread(job);
	delete printer;

	printf("OK\n");
}


void
TestTransportMemoryLeaks()
{
	printf("Testing Transport memory leaks... ");

	BPath path;
	Transport* transport = new Transport(path);
	BMessage msg;
	transport->ListAvailablePorts(&msg);
	delete transport;

	printf("OK\n");
}


int
main(int argc, char** argv)
{
	TestPrintServerAppMemoryLeaks();
	TestPrinterMemoryLeaks();
	TestTransportMemoryLeaks();

	return 0;
}
