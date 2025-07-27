#include <stdio.h>
#include <OS.h>

#include <File.h>
#include <Entry.h>

static int32
set_to_thread(void* data)
{
	BFile* file = (BFile*)data;
	BEntry entry("/boot/home/config/settings/app_server_settings");
	file->SetTo(&entry, B_READ_ONLY);
	return 0;
}


int
main()
{
	BFile file;

	thread_id threads[10];
	for (int i = 0; i < 10; i++) {
		threads[i] = spawn_thread(set_to_thread, "set_to_thread", B_NORMAL_PRIORITY, &file);
	}

	for (int i = 0; i < 10; i++) {
		status_t status;
		wait_for_thread(threads[i], &status);
	}

	return 0;
}
