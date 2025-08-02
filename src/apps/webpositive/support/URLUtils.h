/*
 * Copyright 2023, Haiku, Inc.
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef URL_UTILS_H
#define URL_UTILS_H

#include <BString.h>

class BUrl;

class URLUtils {
public:
	static			bool		IsValid(const BString& url);
	static			bool		IsSearchURL(const BString& url);
	static			bool		IsProxyAddress(const BString& url);
};

#endif // URL_UTILS_H
