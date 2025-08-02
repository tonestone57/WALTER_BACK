/*
 * Copyright 2025, Your Name <your@email.com>
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <OS.h>
#include <Messenger.h>

class BMessage;

class DataLoader {
public:
						DataLoader(BMessenger target);
						~DataLoader();

			void		Start();

private:
	static	int32		_ThreadFunction(void* data);
			int32		_LoadData();

			thread_id	fThread;
			BMessenger	fTarget;
};

#endif // DATA_LOADER_H
