/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "CredentialsStorage.h"

#include <new>
#include <stdio.h>
#include <bcrypt.h>

#include <Autolock.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <Message.h>
#include <Path.h>

#include "BrowserApp.h"


Credentials::Credentials()
	:
	fUsername(),
	fPasswordHash()
{
}


Credentials::Credentials(const BString& username, const BString& password)
	:
	fUsername(username)
{
	char salt[BCRYPT_SALT_LEN];
	bcrypt_gensalt(12, salt);
	bcrypt_hashpw(password, salt, fPasswordHash.LockBuffer(BCRYPT_HASH_LEN));
	fPasswordHash.UnlockBuffer();
}


Credentials::Credentials(const Credentials& other)
{
	*this = other;
}


Credentials::Credentials(const BMessage* archive)
{
	if (archive == NULL)
		return;
	archive->FindString("username", &fUsername);
	archive->FindString("password_hash", &fPasswordHash);
}


Credentials::~Credentials()
{
}


status_t
Credentials::Archive(BMessage* archive) const
{
	if (archive == NULL)
		return B_BAD_VALUE;
	status_t status = archive->AddString("username", fUsername);
	if (status == B_OK)
		status = archive->AddString("password_hash", fPasswordHash);
	return status;
}


Credentials&
Credentials::operator=(const Credentials& other)
{
	if (this == &other)
		return *this;

	fUsername = other.fUsername;
	fPasswordHash = other.fPasswordHash;

	return *this;
}


bool
Credentials::operator==(const Credentials& other) const
{
	if (this == &other)
		return true;

	return fUsername == other.fUsername && fPasswordHash == other.fPasswordHash;
}


bool
Credentials::operator!=(const Credentials& other) const
{
	return !(*this == other);
}


const BString&
Credentials::Username() const
{
	return fUsername;
}


bool
Credentials::CheckPassword(const BString& password) const
{
	return fPasswordHash == NULL
		|| bcrypt_checkpw(password, fPasswordHash) == 0;
}


// #pragma mark - CredentialsStorage


CredentialsStorage
CredentialsStorage::sPersistentInstance(true);


CredentialsStorage
CredentialsStorage::sSessionInstance(false);


CredentialsStorage::CredentialsStorage(bool persistent)
	:
	BLocker(persistent ? "persistent credential storage"
		: "credential storage"),
	fCredentialMap(),
	fSettingsLoaded(false),
	fPersistent(persistent)
{
}


CredentialsStorage::~CredentialsStorage()
{
	_SaveSettings();
}


/*static*/ CredentialsStorage*
CredentialsStorage::SessionInstance()
{
	return &sSessionInstance;
}


/*static*/ CredentialsStorage*
CredentialsStorage::PersistentInstance()
{
	if (sPersistentInstance.Lock()) {
		sPersistentInstance._LoadSettings();
		sPersistentInstance.Unlock();
	}
	return &sPersistentInstance;
}


bool
CredentialsStorage::Contains(const HashString& key)
{
	BAutolock _(this);

	return fCredentialMap.ContainsKey(key);
}


status_t
CredentialsStorage::PutCredentials(const HashString& key,
	const Credentials& credentials)
{
	BAutolock _(this);

	return fCredentialMap.Put(key, credentials);
}


Credentials
CredentialsStorage::GetCredentials(const HashString& key)
{
	BAutolock _(this);

	return fCredentialMap.Get(key);
}


// #pragma mark - private


void
CredentialsStorage::_LoadSettings()
{
	if (!fPersistent || fSettingsLoaded)
		return;

	fSettingsLoaded = true;

	BFile settingsFile;
	if (_OpenSettingsFile(settingsFile, B_READ_ONLY)) {
		BMessage settingsArchive;
		settingsArchive.Unflatten(&settingsFile);
		BMessage credentialsArchive;
		for (int32 i = 0; settingsArchive.FindMessage("credentials", i,
				&credentialsArchive) == B_OK; i++) {
			BString key;
			if (credentialsArchive.FindString("key", &key) == B_OK) {
				Credentials credentials(&credentialsArchive);
				fCredentialMap.Put(key.String(), credentials);
			}
		}
	}
}


void
CredentialsStorage::_SaveSettings() const
{
	BFile settingsFile;
	if (!_OpenSettingsFile(settingsFile,
			B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY)) {
		fprintf(stderr, "Failed to open credentials settings file for writing.\n");
		return;
	}

	BMessage settingsArchive;
	BMessage credentialsArchive;
	CredentialMap::Iterator iterator = fCredentialMap.GetIterator();
	while (iterator.HasNext()) {
		const CredentialMap::Entry& entry = iterator.Next();
		if (entry.value.Archive(&credentialsArchive) != B_OK
			|| credentialsArchive.AddString("key",
				entry.key.GetString()) != B_OK) {
			break;
		}
		if (settingsArchive.AddMessage("credentials",
				&credentialsArchive) != B_OK) {
			break;
		}
		credentialsArchive.MakeEmpty();
	}
	if (settingsArchive.Flatten(&settingsFile) != B_OK)
		fprintf(stderr, "Failed to save credentials settings.\n");
}


bool
CredentialsStorage::_OpenSettingsFile(BFile& file, uint32 mode) const
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK
		|| path.Append(kApplicationName) != B_OK
		|| path.Append("CredentialsStorage") != B_OK) {
		return false;
	}
	status_t status = file.SetTo(path.Path(), mode);
	if (status == B_OK && (mode & B_CREATE_FILE) != 0)
		file.SetPermissions(0600);

	return status == B_OK;
}

