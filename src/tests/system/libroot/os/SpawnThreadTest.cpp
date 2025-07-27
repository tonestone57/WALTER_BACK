#include <stdio.h>
#include <OS.h>

int
main()
{
	thread_id thread = spawn_thread(NULL, "test", B_NORMAL_PRIORITY, NULL);
	if (thread != B_NO_MEMORY)
		return 1;

	return 0;
}
