/*
 * Copyright 2023, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <Looper.h>
#include <Locker.h>
#include <ObjectList.h>

#include "SettingsMessage.h"

class BMessage;

class SessionManager : public BLooper, public BLocker {
public:
								SessionManager(const char* name,
									uint32 priority, BLooper* target);
	virtual						~SessionManager();

	virtual void				MessageReceived(BMessage* message);

			status_t			SaveSession(BMessage* session);
			status_t			LoadSession(BMessage* session);

private:
			SettingsMessage*	fSession;
			BLooper*			fTarget;
};


#endif // SESSION_MANAGER_H
