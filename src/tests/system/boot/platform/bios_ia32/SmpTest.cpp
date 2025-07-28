#include <stdio.h>
#include <OS.h>

#include "smp.h"

extern "C" void smp_start_kernel(void);

int
main()
{
	gKernelArgs.num_cpus = 1024;
	smp_start_kernel();
	return 0;
}
