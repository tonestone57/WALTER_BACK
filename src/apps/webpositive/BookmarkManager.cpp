/*
 * Copyright 2023, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 */


#include "BookmarkManager.h"

#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <memory>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <NodeInfo.h>
#include <Roster.h>
#include <String.h>
#include <View.h>

#include "BrowserApp.h"
#include "BrowserWindow.h"


#include <ObjectList.h>


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "BookmarkManager"


BookmarkManager::BookmarkManager()
	:
	fBookmarkURLs(20)
{
	_LoadBookmarkURLs();
}


BookmarkManager::~BookmarkManager()
{
}


status_t
BookmarkManager::GetBookmarkPath(BPath& path) const
{
	status_t ret = find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	if (ret != B_OK)
		return ret;

	ret = path.Append(kApplicationName);
	if (ret != B_OK)
		return ret;

	ret = path.Append("Bookmarks");
	if (ret != B_OK)
		return ret;

	return create_directory(path.Path(), 0777);
}


void
BookmarkManager::CreateBookmark(BrowserWindow* window)
{
	BString fileName;
		// A name will be derived from the title later.
	BString title(window->CurrentWebView()->MainFrameTitle());
	BString url(window->CurrentWebView()->MainFrameURL());
	BPath path;
	status_t status = GetBookmarkPath(path);

	std::unique_ptr<BBitmap> miniIcon;
	std::unique_ptr<BBitmap> largeIcon;
	PageUserData* userData = static_cast<PageUserData*>(window->CurrentWebView()->GetUserData());
	if (userData != NULL && userData->PageIcon() != NULL) {
		miniIcon = std::make_unique<BBitmap>(userData->PageIcon());
		// TODO:  retrieve the large icon too, once PageUserData can provide it.
	}

	if (status == B_OK)
		CreateBookmark(path, fileName, title, url, miniIcon.get(), largeIcon.get());
	else {
		fprintf(stderr, "BookmarkManager: There was an error retrieving the "
			"bookmark folder: %s\n", strerror(status));
	}
	return;
}


void
BookmarkManager::CreateBookmark(const BPath& path, BString fileName,
	const BString& title, const BString& url, BBitmap* miniIcon,
	BBitmap* largeIcon)
{
	// Determine the file name if one was not provided
	bool presetFileName = true;
	if (fileName.IsEmpty() == true) {
		presetFileName = false;
		fileName = title;
		if (fileName.Length() == 0) {
			fileName = url;
			int32 leafPos = fileName.FindLast('/');
			if (leafPos >= 0)
				fileName.Remove(0, leafPos + 1);
		}
		fileName.ReplaceAll('/', '-');
		fileName.Truncate(B_FILE_NAME_LENGTH - 1);
	}

	BPath entryPath(path);
	status_t status = entryPath.Append(fileName);
	BEntry entry;
	if (status == B_OK)
		status = entry.SetTo(entryPath.Path(), true);

	// There are several reasons why an entry matching the path argument could already exist.
	if (status == B_OK && entry.Exists() == true) {
		off_t size;
		entry.GetSize(&size);
		char attrName[B_ATTR_NAME_LENGTH];
		BNode node(&entry);
		status_t attrStatus = node.GetNextAttrName(attrName);
		if (strcmp(attrName, "_trk/pinfo_le") == 0)
			attrStatus = node.GetNextAttrName(attrName);

		if (presetFileName == true && size == 0 && attrStatus == B_ENTRY_NOT_FOUND) {
			// Tracker's drag-and-drop routine created an empty entry for us to fill in.
			// Go ahead and write to the existing entry.
		} else {
			if (_CheckBookmarkExists(url))
				return;

			BEntry entry(entryPath.Path(), true);
			if (entry.Exists()) {
				// Find a unique name for the bookmark.
				int32 tries = 1;
				BString originalFileName = fileName;
				while (entry.Exists()) {
					fileName = originalFileName;
					fileName << " " << tries++;
					entryPath = path;
					status = entryPath.Append(fileName);
					if (status != B_OK)
						break;
					status = entry.SetTo(entryPath.Path(), true);
					if (status != B_OK)
						break;
				}
			}
		}
	}

	BFile bookmarkFile;
	if (status == B_OK) {
		status = bookmarkFile.SetTo(&entry,
			B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY);
	}

	// Write bookmark meta data
	if (status == B_OK)
		status = bookmarkFile.WriteAttrString("META:url", &url);
	if (status == B_OK) {
		bookmarkFile.WriteAttrString("META:title", &title);
	}

	BNodeInfo nodeInfo(&bookmarkFile);
	if (status == B_OK) {
		status = nodeInfo.SetType("application/x-vnd.Be-bookmark");
		// Replace the standard Be-bookmark file icons with the argument icons,
		// if any were provided.
		if (status == B_OK) {
			status_t ret = B_OK;
			if (miniIcon != NULL) {
				ret = nodeInfo.SetIcon(miniIcon, B_MINI_ICON);
				if (ret != B_OK)
					fprintf(stderr, "BookmarkManager: There was an error "
						"storing the mini icon for the bookmark: %s\n", strerror(ret));
			}
			if (largeIcon != NULL && ret == B_OK)
				ret = nodeInfo.SetIcon(largeIcon, B_LARGE_ICON);
			else if (largeIcon == NULL && miniIcon != NULL && ret == B_OK) {
				// If largeIcon is not available but miniIcon is, use a magnified miniIcon instead.
				BBitmap substituteLargeIcon(BRect(0, 0, 31, 31),
					B_BITMAP_NO_SERVER_LINK, miniIcon->ColorSpace());
				BView offscreenView(substituteLargeIcon.Bounds(), "offscreen",
					0, 0);
				substituteLargeIcon.AddChild(&offscreenView);
				if (offscreenView.LockLooper()) {
					offscreenView.DrawBitmap(miniIcon, miniIcon->Bounds(),
						substituteLargeIcon.Bounds());
					offscreenView.Sync();
					offscreenView.UnlockLooper();
				}
				substituteLargeIcon.RemoveChild(&offscreenView);
				ret = nodeInfo.SetIcon(&substituteLargeIcon, B_LARGE_ICON);
			} else
				ret = B_OK;
			if (ret != B_OK) {
				fprintf(stderr, "BookmarkManager: There was an error "
					"storing the large icon for the bookmark: %s\n", strerror(ret));
			}
		}
	}

	if (status != B_OK) {
		fprintf(stderr, "BookmarkManager: There was an error creating the "
			"bookmark file: %s\n", strerror(status));
	} else
		fBookmarkURLs.AddItem(new BString(url));
}


void
BookmarkManager::CreateBookmark(BMessage* message)
{
		entry_ref ref;
		BMessage originatorData;
		const char* url;
		const char* title;
		bool validData = (message->FindRef("directory", &ref) == B_OK
			&& message->FindMessage("be:originator-data", &originatorData) == B_OK
			&& originatorData.FindString("url", &url) == B_OK
			&& originatorData.FindString("title", &title) == B_OK);

		// Optional data
		const char* fileName;
		if (message->FindString("name", &fileName) != B_OK) {
			// This string is only present if the message originated from Tracker (drag and drop).
			fileName = "";
		}
		std::unique_ptr<BBitmap> miniIcon;
		std::unique_ptr<BBitmap> largeIcon;
		BMessage miniIconArchive;
		if (originatorData.FindMessage("miniIcon", &miniIconArchive) == B_OK)
			miniIcon = std::make_unique<BBitmap>(&miniIconArchive);
		BMessage largeIconArchive;
		if (originatorData.FindMessage("largeIcon", &largeIconArchive) == B_OK)
			largeIcon = std::make_unique<BBitmap>(&largeIconArchive);

		if (validData == true) {
			CreateBookmark(BPath(&ref), BString(fileName), BString(title), BString(url),
				miniIcon.get(), largeIcon.get());
		} else {
			fprintf(stderr, "BookmarkManager: There was an error setting up "
				"the bookmark.\n");
		}
		return;
}


void
BookmarkManager::ShowBookmarks()
{
	BPath path;
	entry_ref ref;
	status_t status = GetBookmarkPath(path);
	if (status == B_OK)
		status = get_ref_for_path(path.Path(), &ref);
	if (status == B_OK)
		status = be_roster->Launch(&ref);

	if (status != B_OK && status != B_ALREADY_RUNNING) {
		fprintf(stderr, "BookmarkManager: There was an error trying to "
			"show the Bookmarks folder: %s\n", strerror(status));
		return;
	}
}


void
BookmarkManager::_LoadBookmarkURLs()
{
	BPath path;
	if (GetBookmarkPath(path) != B_OK)
		return;

	BDirectory directory(path.Path());
	uint32 count = 0;
	_AddBookmarkURLsRecursively(directory, NULL, count);
}


bool
BookmarkManager::_CheckBookmarkExists(const BString& url) const
{
	for (int32 i = 0; i < fBookmarkURLs.CountItems(); i++) {
		if (*fBookmarkURLs.ItemAt(i) == url)
			return true;
	}
	return false;
}


bool
BookmarkManager::_ReadURLAttr(BFile& bookmarkFile, BString& url) const
{
	return bookmarkFile.InitCheck() == B_OK
		&& bookmarkFile.ReadAttrString("META:url", &url) == B_OK;
}


void
BookmarkManager::_AddBookmarkURLsRecursively(BDirectory& directory,
	BMessage* message, uint32& addedCount)
{
	BObjectList<BDirectory, true> directories(10);
	directories.AddItem(new BDirectory(directory));

	while (!directories.IsEmpty()) {
		BDirectory* currentDir = directories.RemoveItemAt(0);
		BEntry entry;
		while (currentDir->GetNextEntry(&entry) == B_OK) {
			if (entry.IsDirectory()) {
				directories.AddItem(new BDirectory(&entry));
			} else {
				BString storedURL;
				BFile file(&entry, B_READ_ONLY);
				if (_ReadURLAttr(file, storedURL)) {
					if (message != NULL)
						message->AddString("url", storedURL.String());
					else
						fBookmarkURLs.AddItem(new BString(storedURL));
					addedCount++;
				}
			}
		}
		delete currentDir;
	}
}
