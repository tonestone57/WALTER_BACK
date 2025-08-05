/*
 * Copyright 2025, Haiku, Inc.
 * Distributed under the terms of the MIT License.
 */

#include "URLHandler.h"

#include <Application.h>
#include <Roster.h>
#include <Url.h>
#include <WebView.h>
#include <Alert.h>

#include "BrowserApp.h"
#include "SettingsMessage.h"

#include <cctype>


// A subset of the public suffix list.
static const char* kValidTLDs[] = {
	"ac", "ad", "ae", "aero", "af", "ag", "ai", "al", "am", "ao", "aq", "ar",
	"arpa", "as", "asia", "at", "au", "aw", "ax", "az", "ba", "bb", "bd", "be",
	"bf", "bg", "bh", "bi", "biz", "bj", "bm", "bn", "bo", "br", "bs", "bt",
	"bv", "bw", "by", "bz", "ca", "cat", "cc", "cd", "cf", "cg", "ch", "ci",
	"ck", "cl", "cm", "cn", "co", "com", "coop", "cr", "cu", "cv", "cw", "cx",
	"cy", "cz", "de", "dj", "dk", "dm", "do", "dz", "ec", "edu", "ee", "eg",
	"er", "es", "et", "eu", "fi", "fj", "fk", "fm", "fo", "fr", "ga", "gb",
	"gd", "ge", "gf", "gg", "gh", "gi", "gl", "gm", "gn", "gov", "gp", "gq",
	"gr", "gs", "gt", "gu", "gw", "gy", "hk", "hm", "hn", "hr", "ht", "hu",
	"id", "ie", "il", "im", "in", "info", "int", "io", "iq", "ir", "is", "it",
	"je", "jm", "jo", "jobs", "jp", "ke", "kg", "kh", "ki", "km", "kn", "kp",
	"kr", "kw", "ky", "kz", "la", "lb", "lc", "li", "lk", "lr", "ls", "lt",
	"lu", "lv", "ly", "ma", "mc", "md", "me", "mg", "mh", "mil", "mk", "ml",
	"mm", "mn", "mo", "mobi", "mp", "mq", "mr", "ms", "mt", "mu", "museum",
	"mv", "mw", "mx", "my", "mz", "na", "name", "nc", "ne", "net", "nf", "ng",
	"ni", "nl", "no", "np", "nr", "nu", "nz", "om", "onion", "org", "pa", "pe",
	"pf", "pg", "ph", "pk", "pl", "pm", "pn", "post", "pr", "pro", "ps", "pt",
	"pw", "py", "qa", "re", "ro", "rs", "ru", "rw", "sa", "sb", "sc", "sd",
	"se", "sg", "sh", "si", "sj", "sk", "sl", "sm", "sn", "so", "sr", "ss",
	"st", "su", "sv", "sx", "sy", "sz", "tc", "td", "tel", "tf", "tg", "th",
	"tj", "tk", "tl", "tm", "tn", "to", "tr", "tt", "tv", "tw", "tz", "ua",
	"ug", "uk", "us", "uy", "uz", "va", "vc", "ve", "vg", "vi", "vn", "vu",
	"wf", "ws", "xxx", "ye", "yt", "za", "zm", "zw"
};

static const char* kHandledProtocols[] = {
	"http",
	"https",
	"ftp",
	"file",
	"about",
	"data",
	"gopher"
};


URLHandler::URLHandler(BWebView* webView, SettingsMessage* protocolHandlers,
	const BString& searchPageURL)
	:
	fWebView(webView),
	fProtocolHandlers(protocolHandlers),
	fSearchPageURL(searchPageURL)
{
}


URLHandler::~URLHandler()
{
}


void
URLHandler::Handle(const BString& url)
{
	if (url.Length() == 0)
		return;

	BUrl urlObject(url.String(), true);
	if (urlObject.Protocol().Length() > 0) {
		// This is a URL with a protocol. Let's see if we can handle it.
		bool handled = false;
		for (unsigned int i = 0; i < sizeof(kHandledProtocols) / sizeof(char*);
				i++) {
			if (urlObject.Protocol() == kHandledProtocols[i]) {
				handled = true;
				break;
			}
		}

		if (handled) {
			_VisitURL(url);
			return;
		} else {
			// There is what looks like a protocol, but one we don't know.
			// Ask the BRoster if there is a matching filetype and app which
			// can handle it.
			BString protocol = urlObject.Protocol();
			BString storedChoice = fProtocolHandlers->GetValue(protocol.String(), "");

			if (storedChoice == "deny")
				return;

			if (storedChoice != "allow") {
				BString alertText("The protocol '");
				alertText << protocol << "' is not handled by WebPositive. "
					"Do you want to launch an external application to handle "
					"this URL?";
				BAlert* alert = new BAlert("Unknown Protocol", alertText,
					"Deny", "Allow Once", "Always Allow");
				alert->SetShortcut(0, B_ESCAPE);
				int32 choice = alert->Go();

				if (choice == 0) { // Deny
					fProtocolHandlers->SetValue(protocol.String(), "deny");
					return;
				}
				if (choice == 2) // Always Allow
					fProtocolHandlers->SetValue(protocol.String(), "allow");
			}

			BString temp;
			temp = "application/x-vnd.Be.URL.";
			temp += protocol;

			const char* argv[] = { url.String(), NULL };

			if (be_roster->Launch(temp.String(), 1, argv) == B_OK)
				return;
		}
	}

	// If there is no protocol, check if it's a URL.
	if (_IsURL(url)) {
		BString urlWithHttp = BString("http://").Append(url);
		_VisitURL(urlWithHttp);
		return;
	}

	// If all else fails, treat it as a search query.
	_VisitSearchEngine(url);
}


BString
URLHandler::_EncodeURIComponent(const BString& search)
{
	static const char* hex = "0123456789ABCDEF";
	BString result;
	for (int32 i = 0; i < search.Length(); i++) {
		char c = search[i];
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
			result += c;
		else {
			result += '%';
			result += hex[c >> 4];
			result += hex[c & 0xf];
		}
	}
	return result;
}


void
URLHandler::_VisitURL(const BString& url)
{
	if (url.Length() > 0)
		fWebView->LoadURL(url.String());
}


void
URLHandler::_VisitSearchEngine(const BString& search)
{
	BString searchQuery = search;

	BString searchPrefix;
	search.CopyCharsInto(searchPrefix, 0, 2);

	// Default search URL
	BString engine(fSearchPageURL);
	if (engine.Length() == 0)
		engine = "https://duckduckgo.com/?q=%s";

	engine.ReplaceAll("%s", _EncodeURIComponent(searchQuery));
	_VisitURL(engine);
}


bool
URLHandler::_IsURL(const BString& string)
{
	// Check for an IPv4 address.
	int a, b, c, d;
	if (sscanf(string.String(), "%d.%d.%d.%d", &a, &b, &c, &d) == 4) {
		if (a >= 0 && a <= 255 && b >= 0 && b <= 255 && c >= 0 && c <= 255
			&& d >= 0 && d <= 255) {
			return true;
		}
	}

	// Check for a dot and a valid TLD.
	int32 dot = string.FindLast('.');
	if (dot < 0)
		return false;

	BString tld;
	string.CopyInto(tld, dot + 1, string.Length() - dot - 1);
	tld.ToLower();

	for (uint32 i = 0; i < sizeof(kValidTLDs) / sizeof(char*); i++) {
		if (tld == kValidTLDs[i])
			return true;
	}

	return false;
}
