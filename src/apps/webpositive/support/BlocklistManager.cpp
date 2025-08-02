/*
 * Copyright 2023, Haiku, Inc.
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#include "BlocklistManager.h"

#include <File.h>
#include <FindDirectory.h>
#include <Path.h>
#include <String.h>
#include <Url.h>

#include "BrowserApp.h"


BlocklistManager::BlocklistManager()
{
	_LoadBlocklist();
}


BlocklistManager::~BlocklistManager()
{
}


bool
BlocklistManager::IsBlocked(const BUrl& url)
{
	return fBlocklist.find(url.Host()) != fBlocklist.end();
}


void
BlocklistManager::_LoadBlocklist()
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK)
		return;

	path.Append(kApplicationName);
	path.Append("Blocklist");

	BFile file(path.Path(), B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;

	char line[1024];
	while (file.Read(line, sizeof(line)) > 0) {
		BString host(line);
		host.RemoveAll("\n");
		fBlocklist.insert(host);
	}
}
