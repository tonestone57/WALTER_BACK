#include <stdio.h>
#include <OS.h>

#include "AppServer.h"
#include "Desktop.h"
#include "EventDispatcher.h"
#include "Window.h"

static int32
create_desktop_thread(void* data)
{
	AppServer* server = (AppServer*)data;
	server->_CreateDesktop(0, NULL);
	return 0;
}


static int32
set_focus_window_thread(void* data)
{
	Desktop* desktop = (Desktop*)data;
	Window* window = new Window(BRect(0, 0, 100, 100), "test", B_TITLED_WINDOW, 0, 0);
	desktop->SetFocusWindow(window);
	return 0;
}


static int32
send_message_thread(void* data)
{
	EventDispatcher* dispatcher = (EventDispatcher*)data;
	BMessage message(B_QUIT_REQUESTED);
	BMessenger messenger;
	dispatcher->_SendMessage(messenger, &message, 1.0);
	return 0;
}


int
main()
{
	AppServer server(NULL);

	thread_id threads[10];
	for (int i = 0; i < 10; i++) {
		threads[i] = spawn_thread(create_desktop_thread, "create_desktop_thread", B_NORMAL_PRIORITY, &server);
	}

	for (int i = 0; i < 10; i++) {
		status_t status;
		wait_for_thread(threads[i], &status);
	}

	Desktop* desktop = server._FindDesktop(0, NULL);
	for (int i = 0; i < 10; i++) {
		threads[i] = spawn_thread(set_focus_window_thread, "set_focus_window_thread", B_NORMAL_PRIORITY, desktop);
	}

	for (int i = 0; i < 10; i++) {
		status_t status;
		wait_for_thread(threads[i], &status);
	}

	EventDispatcher* dispatcher = &desktop->EventDispatcher();
	for (int i = 0; i < 10; i++) {
		threads[i] = spawn_thread(send_message_thread, "send_message_thread", B_NORMAL_PRIORITY, dispatcher);
	}

	for (int i = 0; i < 10; i++) {
		status_t status;
		wait_for_thread(threads[i], &status);
	}

	return 0;
}
