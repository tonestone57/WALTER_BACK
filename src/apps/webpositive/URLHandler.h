/*
 * Copyright 2025, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 */
#ifndef URL_HANDLER_H
#define URL_HANDLER_H

#include <String.h>

class BWebView;
class SettingsMessage;

class URLHandler {
public:
								URLHandler(BWebView* webView,
	URLHandler(SettingsMessage* protocolHandlers, const BString& searchPageURL);
	virtual						~URLHandler();

			void				Handle(const BString& url);
	void SetWebView(BWebView* view);

private:
			BString				_EncodeURIComponent(const BString& string);
			void				_VisitURL(const BString& url);
			void				_VisitSearchEngine(const BString& search);
			bool				_IsURL(const BString& string);

private:
			BWebView*			fWebView;
			SettingsMessage*	fProtocolHandlers;
			BString				fSearchPageURL;
};

#endif // URL_HANDLER_H
