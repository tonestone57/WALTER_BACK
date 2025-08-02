/*
 * Copyright 2023, Haiku, Inc.
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#include "URLUtils.h"

#include <Url.h>

bool
URLUtils::IsValid(const BString& url)
{
	return BUrl(url).IsValid();
}

bool
URLUtils::IsSearchURL(const BString& url)
{
	return url.FindFirst("%s") >= 0;
}

bool
URLUtils::IsProxyAddress(const BString& url)
{
	return BUrl(url).Host().Length() > 0;
}
