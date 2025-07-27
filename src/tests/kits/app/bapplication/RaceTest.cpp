#include <stdio.h>
#include <OS.h>

#include <Application.h>

static int32
create_application_thread(void* data)
{
	new BApplication("application/x-vnd.haiku-test");
	return 0;
}


int
main()
{
	thread_id threads[10];
	for (int i = 0; i < 10; i++) {
		threads[i] = spawn_thread(create_application_thread, "create_application_thread", B_NORMAL_PRIORITY, NULL);
	}

	for (int i = 0; i < 10; i++) {
		status_t status;
		wait_for_thread(threads[i], &status);
	}

	return 0;
}
