/*
 * Copyright 2023, Haiku, Inc.
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef ICON_LOADER_H
#define ICON_LOADER_H

#include <Looper.h>

class BBitmap;
class BHandler;
class BMessage;

class IconLoader : public BLooper {
public:
						IconLoader(BHandler* target);

	virtual void		MessageReceived(BMessage* message);

private:
			BHandler*	fTarget;
};

#endif // ICON_LOADER_H
