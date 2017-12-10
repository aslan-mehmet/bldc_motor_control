#include "server.h"
#include "stdio.h"

int main(void)
{
	if (server_create()) {
		return -1;
	}

	while (1) {
		server_loop();
	}

	return 0;
}
