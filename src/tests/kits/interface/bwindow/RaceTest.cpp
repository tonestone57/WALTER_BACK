#include <stdio.h>
#include <OS.h>

#include <Application.h>
#include <Window.h>

static int32
create_window_thread(void* data)
{
	new BWindow(BRect(0, 0, 100, 100), "test", B_TITLED_WINDOW, 0, 0);
	return 0;
}


int
main()
{
	new BApplication("application/x-vnd.haiku-test");

	thread_id threads[10];
	for (int i = 0; i < 10; i++) {
		threads[i] = spawn_thread(create_window_thread, "create_window_thread", B_NORMAL_PRIORITY, NULL);
	}

	for (int i = 0; i < 10; i++) {
		status_t status;
		wait_for_thread(threads[i], &status);
	}

	return 0;
}
