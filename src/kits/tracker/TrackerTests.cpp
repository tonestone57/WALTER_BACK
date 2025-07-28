/*
 * Copyright 2025, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include <stdio.h>

#include <ContainerWindow.h>
#include <DeskWindow.h>
#include <FindPanel.h>
#include <Tracker.h>

#include "Commands.h"


void
TestContainerWindowQuit()
{
	printf("Testing BContainerWindow::Quit()... ");

	TTracker tracker;
	BContainerWindow* window = new BContainerWindow(tracker.fWindowList, 0,
		B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, 0);
	window->Show();
	window->PostMessage(B_QUIT_REQUESTED);

	printf("OK\n");
}


void
TestDeskWindowMemoryLeak()
{
	printf("Testing BDeskWindow memory leak... ");

	TTracker tracker;
	BDeskWindow* window = new BDeskWindow(tracker.fWindowList, 0);
	window->Show();
	window->PostMessage(B_QUIT_REQUESTED);

	printf("OK\n");
}


void
TestFindPanelMemoryLeak()
{
	printf("Testing FindPanel memory leaks... ");

	FindWindow* findWindow = new FindWindow();
	findWindow->Show();
	findWindow->PostMessage(B_QUIT_REQUESTED);

	printf("OK\n");
}


int
main(int argc, char** argv)
{
	TestContainerWindowQuit();
	TestDeskWindowMemoryLeak();
	TestFindPanelMemoryLeak();

	return 0;
}
