/*
 * Copyright 2023, Haiku, Inc.
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef BLOCKLIST_MANAGER_H
#define BLOCKLIST_MANAGER_H

#include <String.h>
#include <set>

class BUrl;

class BlocklistManager {
public:
								BlocklistManager();
								~BlocklistManager();

			bool				IsBlocked(const BUrl& url);

private:
			void				_LoadBlocklist();

private:
			std::set<BString>	fBlocklist;
};

#endif // BLOCKLIST_MANAGER_H
