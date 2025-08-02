/*
 * Copyright 2023, Haiku, Inc.
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#include "IconLoader.h"

#include <Bitmap.h>
#include <Message.h>

#include "WebViewConstants.h"


IconLoader::IconLoader(BHandler* target)
	:
	BLooper("IconLoader"),
	fTarget(target)
{
	Run();
}


void
IconLoader::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case B_ICON_REQUEST:
		{
			BMessage reply(B_ICON_RESULT);
			BMessage iconArchive;
			if (message->FindMessage("icon", &iconArchive) != B_OK)
				break;

			BBitmap* icon = new BBitmap(&iconArchive);

			reply.AddPointer("icon", icon);
			BMessage msg(*message);
			reply.AddMessage("original", &msg);
			fTarget->Looper()->PostMessage(&reply, fTarget);
			break;
		}
		default:
			BLooper::MessageReceived(message);
	}
}
