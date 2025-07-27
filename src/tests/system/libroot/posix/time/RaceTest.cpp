#include <stdio.h>
#include <OS.h>
#include <time.h>

static int32
tzset_thread(void* data)
{
	tzset();
	return 0;
}


int
main()
{
	thread_id threads[10];
	for (int i = 0; i < 10; i++) {
		threads[i] = spawn_thread(tzset_thread, "tzset_thread", B_NORMAL_PRIORITY, NULL);
	}

	for (int i = 0; i < 10; i++) {
		status_t status;
		wait_for_thread(threads[i], &status);
	}

	return 0;
}
