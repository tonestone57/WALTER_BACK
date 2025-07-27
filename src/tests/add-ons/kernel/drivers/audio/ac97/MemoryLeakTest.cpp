#include <stdio.h>
#include <OS.h>

#include "ac97.h"

static uint16
reg_read(void* cookie, uint8 reg)
{
	return 0;
}


static void
reg_write(void* cookie, uint8 reg, uint16 value)
{
}


int
main()
{
	ac97_dev* dev;
	ac97_attach(&dev, reg_read, reg_write, NULL, 0, 0);
	return 0;
}
