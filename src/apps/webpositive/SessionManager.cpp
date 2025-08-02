/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "SessionManager.h"

#include <Autolock.h>
#include <FindDirectory.h>
#include <Path.h>

#include "BrowserApp.h"


SessionManager::SessionManager(const char* name, uint32 priority,
		BLooper* target)
	:
	BLooper(name, priority, 2),
	fSession(NULL),
	fTarget(target)
{
	BString sessionStorePath = kApplicationName;
	sessionStorePath << "/Session";
	fSession = new SettingsMessage(B_USER_SETTINGS_DIRECTORY,
		sessionStorePath.String());
}


SessionManager::~SessionManager()
{
	delete fSession;
}


void
SessionManager::MessageReceived(BMessage* message)
{
	switch (message->what) {
	case 'save':
	{
		BMessage session;
		if (message->FindMessage("session", &session) == B_OK)
			SaveSession(&session);
		break;
	}
	case 'load':
	{
		BMessage session;
		if (LoadSession(&session) == B_OK) {
			BMessage reply('load');
			reply.AddMessage("session", &session);
			fTarget->PostMessage(&reply);
		}
		break;
	}
	default:
		BLooper::MessageReceived(message);
		break;
	}
}


status_t
SessionManager::SaveSession(BMessage* session)
{
	BAutolock _(this);
	*fSession = *session;
	return B_OK;
}


status_t
SessionManager::LoadSession(BMessage* session)
{
	BAutolock _(this);
	*session = *fSession;
	return B_OK;
}
