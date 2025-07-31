#include "BrowserWindow.h"

#include <Menu.h>

enum {
	MSG_POPULATE_HISTORY_MENU = 'phmn'
};

bool
BrowserWindow::_HistoryMenuHook(BMenu* menu, void* userData)
{
	BrowserWindow* window = static_cast<BrowserWindow*>(userData);
	window->PostMessage(MSG_POPULATE_HISTORY_MENU);
	// Remove the hook so it doesn't get called again.
	menu->SetTrackingHook(NULL, NULL);
	return false;
}
