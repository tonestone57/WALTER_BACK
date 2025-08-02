/*
 * Copyright 2023, Haiku, Inc.
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#include "TabCache.h"

#include <WebView.h>

TabCache::TabCache(int32 maxSize)
	:
	fCache(20, true),
	fMaxSize(maxSize)
{
}


TabCache::~TabCache()
{
}


void
TabCache::Add(BWebView* view)
{
	if (fCache.CountItems() >= fMaxSize) {
		BWebView* oldest = fCache.RemoveItem((int32)0);
		delete oldest;
	}
	fCache.AddItem(view);
}


BWebView*
TabCache::Get()
{
	return fCache.RemoveItem(fCache.CountItems() - 1);
}


void
TabCache::Clear()
{
	fCache.MakeEmpty();
}
