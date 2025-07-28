#include <stdio.h>
#include <OS.h>

#include "anyboot.cpp"

int
main()
{
	int fd = open("test.image", O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (fd < 0)
		return 1;

	for (int i = 0; i < 5; i++) {
		createPartition(fd, i, false, 0xeb, 0, 0);
	}

	close(fd);
	return 0;
}
