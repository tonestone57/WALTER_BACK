/*
 * Copyright 2023, Haiku, Inc.
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef TAB_CACHE_H
#define TAB_CACHE_H

#include <ObjectList.h>

class BWebView;

class TabCache {
public:
						TabCache(int32 maxSize);
						~TabCache();

			void		Add(BWebView* view);
			BWebView*	Get();
			void		Clear();

private:
			BObjectList<BWebView> fCache;
			int32		fMaxSize;
};

#endif // TAB_CACHE_H
