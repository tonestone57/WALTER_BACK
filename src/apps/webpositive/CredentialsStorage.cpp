/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "CredentialsStorage.h"

#include <new>
#include <stdio.h>
#include <stdlib.h>
#include "AES.h"

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
	fPassword(),
	fSalt()
{
}


static void
_convertToB64(char* salt, int size)
{
	const char* b64chars =
		"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	for (int i = 0; i < size; i++)
		salt[i] = b64chars[salt[i] % 64];
	salt[size - 1] = 0;
}


Credentials::Credentials(const BString& username, const BString& password)
	:
	fUsername(username),
	fPassword(password)
{
	char salt[21];
	arc4random_buf(salt, sizeof(salt));
	_convertToB64(salt, sizeof(salt));
	fSalt = salt;
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
	archive->FindString("salt", &fSalt);
	BString encryptedPassword;
	if (archive->FindString("password", &encryptedPassword) == B_OK) {
		const std::vector<unsigned char> key = plusaes::key_from_string("a 16-byte key!!");
		unsigned char iv[16];
		memcpy(iv, fSalt.String(), 16);
		std::vector<unsigned char> decrypted(encryptedPassword.Length());
		unsigned long padded_size = 0;
		plusaes::decrypt_cbc((unsigned char*)encryptedPassword.String(), encryptedPassword.Length(), &key[0], key.size(), &iv, &decrypted[0], decrypted.size(), &padded_size);
		fPassword.SetTo((const char*)decrypted.data(), decrypted.size() - padded_size);
	}
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
		status = archive->AddString("salt", fSalt);
	if (status == B_OK) {
		const std::vector<unsigned char> key = plusaes::key_from_string("a 16-byte key!!");
		unsigned char iv[16];
		memcpy(iv, fSalt.String(), 16);
		const unsigned long encrypted_size = plusaes::get_padded_encrypted_size(fPassword.Length());
		std::vector<unsigned char> encrypted(encrypted_size);
		plusaes::encrypt_cbc((unsigned char*)fPassword.String(), fPassword.Length(), &key[0], key.size(), &iv, &encrypted[0], encrypted.size(), true);
		BString encryptedPassword((const char*)encrypted.data(), encrypted.size());
		status = archive->AddString("password", encryptedPassword);
	}
	return status;
}


Credentials&
Credentials::operator=(const Credentials& other)
{
	if (this == &other)
		return *this;

	fUsername = other.fUsername;
	fPassword = other.fPassword;
	fSalt = other.fSalt;

	return *this;
}


bool
Credentials::operator==(const Credentials& other) const
{
	if (this == &other)
		return true;

	return fUsername == other.fUsername && fPassword == other.fPassword;
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


const BString&
Credentials::Password() const
{
	return fPassword;
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

