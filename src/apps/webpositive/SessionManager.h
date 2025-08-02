/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <Locker.h>
#include <Message.h>
#include <String.h>

#include "SettingsMessage.h"

class SessionManager : public BLooper {
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
