#include <stdio.h>
#include <OS.h>

#include "ConnectionInterface.h"

int
main()
{
	// create a full port
	port_id port = create_port(1, "test");
	write_port(port, 0, NULL, 0);

	// call PostEvent()
	bluetooth_device device;
	device.index = 0;
	hci_event_header event;
	event.ecode = 0;
	event.plen = 0;
	PostEvent(&device, &event, sizeof(event));

	return 0;
}
