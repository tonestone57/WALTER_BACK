/*
 * Copyright 2025, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include <stdio.h>

#include <File.h>
#include <Directory.h>
#include <Entry.h>
#include <Node.h>


void
TestFileResourceLeak()
{
	printf("Testing BFile resource leak... ");

	BFile file;
	BFile file2;
	file = file2;

	printf("OK\n");
}


void
TestDirectoryMemoryLeak()
{
	printf("Testing BDirectory memory leak... ");

	create_directory("/tmp/test_dir", 0777);

	printf("OK\n");
}


void
TestEntryMemoryLeak()
{
	printf("Testing entry_ref memory leak... ");

	entry_ref ref;
	ref.set_name(NULL);

	printf("OK\n");
}


void
TestNodeResourceLeak()
{
	printf("Testing BNode resource leak... ");

	BNode node;
	BNode node2;
	node = node2;

	printf("OK\n");
}


int
main(int argc, char** argv)
{
	TestFileResourceLeak();
	TestDirectoryMemoryLeak();
	TestEntryMemoryLeak();
	TestNodeResourceLeak();

	return 0;
}
