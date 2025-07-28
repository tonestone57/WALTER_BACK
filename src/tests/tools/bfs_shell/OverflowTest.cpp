#include <stdio.h>
#include <OS.h>

#include "bfs.h"
#include "bfs_control.h"

namespace FSShell {
fssh_status_t command_checkfs(int argc, const char* const* argv);
}

int
main()
{
	const char* argv[] = { "checkfs", NULL };
	FSShell::command_checkfs(1, argv);
	return 0;
}
