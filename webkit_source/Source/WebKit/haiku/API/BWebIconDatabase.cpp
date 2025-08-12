/*
 * Copyright (C) 2024 Haiku, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with a a distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "BWebIconDatabase.h"

#include <Bitmap.h>
#include <Directory.h>
#include <FindDirectory.h>
#include <Path.h>
#include <sqlite3.h>
#include <Message.h>

BWebIconDatabase* BWebIconDatabase::sDefault = nullptr;

BWebIconDatabase* BWebIconDatabase::Default()
{
    if (!sDefault)
        sDefault = new BWebIconDatabase();
    return sDefault;
}

BWebIconDatabase::BWebIconDatabase()
    : m_db(nullptr)
{
    BPath path;
    if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
        path.Append("webkit/icondatabase.db");
        SetPath(path.Path());
    }
}

BWebIconDatabase::~BWebIconDatabase()
{
    if (m_db)
        sqlite3_close(m_db);
    sDefault = nullptr;
}

void BWebIconDatabase::SetPath(const BString& path)
{
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }

    m_path = path;

    if (sqlite3_open(path.String(), &m_db) != SQLITE_OK) {
        // TODO: report error
        return;
    }

    // Create table if it doesn't exist
    const char* createTableSQL = "CREATE TABLE IF NOT EXISTS IconDatabase ("
                                 "url TEXT NOT NULL ON CONFLICT FAIL UNIQUE ON CONFLICT REPLACE,"
                                 "icon BLOB);";
    if (sqlite3_exec(m_db, createTableSQL, NULL, NULL, NULL) != SQLITE_OK) {
        // TODO: report error
        return;
    }
}

BString BWebIconDatabase::Path() const
{
    return m_path;
}

BBitmap* BWebIconDatabase::IconForURL(const BString& url)
{
    if (!m_db)
        return nullptr;

    sqlite3_stmt* statement;
    const char* getIconSQL = "SELECT icon FROM IconDatabase WHERE url = ?";
    if (sqlite3_prepare_v2(m_db, getIconSQL, -1, &statement, NULL) != SQLITE_OK) {
        // TODO: report error
        return nullptr;
    }

    sqlite3_bind_text(statement, 1, url.String(), -1, SQLITE_STATIC);

    BBitmap* bitmap = nullptr;
    if (sqlite3_step(statement) == SQLITE_ROW) {
        const void* blob = sqlite3_column_blob(statement, 0);
        int blobSize = sqlite3_column_bytes(statement, 0);

        BMessage msg;
        if (msg.Unflatten((const char*)blob, blobSize) == B_OK) {
            bitmap = new BBitmap(&msg);
        }
    }

    sqlite3_finalize(statement);
    return bitmap;
}

void BWebIconDatabase::SetIconForURL(const BString& url, const BBitmap* icon)
{
    if (!m_db)
        return;

    BMessage msg;
    if (icon->Archive(&msg) != B_OK)
        return;

    char* buffer;
    ssize_t size;
    msg.Flatten(&buffer, &size);

    sqlite3_stmt* statement;
    const char* setIconSQL = "INSERT OR REPLACE INTO IconDatabase (url, icon) VALUES (?, ?)";
    if (sqlite3_prepare_v2(m_db, setIconSQL, -1, &statement, NULL) != SQLITE_OK) {
        // TODO: report error
        return;
    }

    sqlite3_bind_text(statement, 1, url.String(), -1, SQLITE_STATIC);
    sqlite3_bind_blob(statement, 2, buffer, size, SQLITE_TRANSIENT);

    if (sqlite3_step(statement) != SQLITE_DONE) {
        // TODO: report error
    }

    sqlite3_finalize(statement);
    delete[] buffer;
}

void BWebIconDatabase::Clear()
{
    if (!m_db)
        return;

    const char* clearTableSQL = "DELETE FROM IconDatabase;";
    if (sqlite3_exec(m_db, clearTableSQL, NULL, NULL, NULL) != SQLITE_OK) {
        // TODO: report error
        return;
    }
}
