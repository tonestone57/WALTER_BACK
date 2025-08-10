/*
 * Copyright (C) 2024 Your Name <your@email.com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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

#include "config.h"
#include "WebPreferencesStore.h"

#include <wtf/HashMap.h>
#include <wtf/text/StringHash.h>

namespace WebKit {

static HashMap<String, String> s_stringPreferences;
static HashMap<String, bool> s_boolPreferences;
static HashMap<String, uint32_t> s_uint32Preferences;
static HashMap<String, double> s_doublePreferences;

void WebPreferencesStore::platformInitialize(const String&, const String&)
{
}

void WebPreferencesStore::platformSetValue(const String& key, const String& value, bool)
{
    s_stringPreferences.set(key, value);
}

void WebPreferencesStore::platformSetValue(const String& key, bool value, bool)
{
    s_boolPreferences.set(key, value);
}

void WebPreferencesStore::platformSetValue(const String& key, uint32_t value, bool)
{
    s_uint32Preferences.set(key, value);
}

void WebPreferencesStore::platformSetValue(const String& key, double value, bool)
{
    s_doublePreferences.set(key, value);
}

bool WebPreferencesStore::platformGetValue(const String& key, String& value)
{
    if (s_stringPreferences.contains(key)) {
        value = s_stringPreferences.get(key);
        return true;
    }
    return false;
}

bool WebPreferencesStore::platformGetValue(const String& key, bool& value)
{
    if (s_boolPreferences.contains(key)) {
        value = s_boolPreferences.get(key);
        return true;
    }
    return false;
}

bool WebPreferencesStore::platformGetValue(const String& key, uint32_t& value)
{
    if (s_uint32Preferences.contains(key)) {
        value = s_uint32Preferences.get(key);
        return true;
    }
    return false;
}

bool WebPreferencesStore::platformGetValue(const String& key, double& value)
{
    if (s_doublePreferences.contains(key)) {
        value = s_doublePreferences.get(key);
        return true;
    }
    return false;
}

} // namespace WebKit
