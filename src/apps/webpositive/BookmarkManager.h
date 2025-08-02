/*
 * Copyright 2023, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 */
#ifndef BOOKMARK_MANAGER_H
#define BOOKMARK_MANAGER_H


#include <Path.h>
#include <ObjectList.h>
#include <String.h>

#include "PageUserData.h"

class BBitmap;
class BDirectory;
class BFile;
class BMessage;
class BrowserWindow;


class BookmarkManager {
public:
								BookmarkManager();
	virtual						~BookmarkManager();

			status_t			GetBookmarkPath(BPath& path) const;
			void				CreateBookmark(BrowserWindow* window);
			void				CreateBookmark(const BPath& path,
									BString fileName, const BString& title,
									const BString& url, BBitmap* miniIcon,
									BBitmap* largeIcon);
			void				CreateBookmark(BMessage* message);
			void				ShowBookmarks();

			bool				_ReadURLAttr(BFile& bookmarkFile,
									BString& url) const;
			void				_AddBookmarkURLsRecursively(
									BDirectory& directory,
									BMessage* message,
									uint32& addedCount);

private:
			void				_LoadBookmarkURLs();
			bool				_CheckBookmarkExists(const BString& url) const;

private:
			BObjectList<BString>	fBookmarkURLs;
			bool				fBookmarksLoaded;
};


#endif // BOOKMARK_MANAGER_H
