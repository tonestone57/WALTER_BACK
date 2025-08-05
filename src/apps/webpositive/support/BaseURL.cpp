/*
 * Copyright 2010 Stephan Aßmus <superstippi@gmx.de>
 * Copyright 2023, Haiku, Inc. All rights reserved.
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#include "BaseURL.h"


BString
baseURL(const BString string)
{
	if (string.IsEmpty())
		return BString();

	int32 protocolEnd = string.FindFirst("://");
	if (protocolEnd < 0)
		return BString();

	int32 baseURLStart = protocolEnd + 3;
	if (baseURLStart >= string.Length())
		return BString();

	int32 baseURLEnd = string.FindFirst("/", baseURLStart);
	if (baseURLEnd < 0) {
		BString result(string);
		result.Remove(0, baseURLStart);
		return result;
	}

	BString result;
	result.SetTo(string, baseURLEnd - baseURLStart);
	result.Remove(0, baseURLStart);
	return result;
}
