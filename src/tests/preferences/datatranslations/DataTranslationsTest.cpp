#include <stdio.h>
#include <Application.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <Path.h>

#include "DataTranslations.h"

int
main()
{
	new BApplication("application/x-vnd.haiku-test");

	BPath path;
	find_directory(B_USER_NONPACKAGED_ADDONS_DIRECTORY, &path, true);
	BDirectory target;
	target.SetTo(path.Path());
	target.CreateDirectory("test_translators", &target);

	BFile source("/boot/home/config/settings/app_server_settings", B_READ_ONLY);
	BEntry entry("/boot/home/config/settings/app_server_settings");

	DataTranslationsApplication app;
	app._Install(target, entry);

	BPath newPath(path.Path());
	newPath.Append("test_translators/app_server_settings");
	BEntry newEntry(newPath.Path());
	if (!newEntry.Exists())
		return 1;

	return 0;
}
