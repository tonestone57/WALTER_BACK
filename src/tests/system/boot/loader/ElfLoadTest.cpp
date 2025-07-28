#include <stdio.h>
#include <OS.h>

#include "elf.h"

int
main()
{
	preloaded_image* image;
	elf_load_image(0, &image);
	return 0;
}
