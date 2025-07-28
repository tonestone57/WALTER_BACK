#include <stdio.h>
#include <Application.h>
#include "BluetoothMain.h"

int
main()
{
	BluetoothApplication app;
	app.ReadyToRun();
	if (app.fWindow == NULL)
		return 1;
	return 0;
}
