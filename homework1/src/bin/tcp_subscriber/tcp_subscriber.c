/*
 *	<Header stuff goes here>
 */
#include <stdio.h>
#include <stdlib.h>
#include "tcpnitslib.h"
#define MY_PORT	7640

int
main(int argc, char *argv[])
{
	/*
	 * check args.
	 */
	struct in_addr *host;
	/*
	 * make sure to fill in the host information correctly.
	 */
	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <article> [opt IP addr]\n", argv[0]);
		exit (1);
	}
	/*
	 * do your stuff here...just calling setup_subscriber
	 * in this example for fun.
	 */
	if (setup_subscriber (host, MY_PORT) == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "Stupid idiot, you screwed up. You know nothing.\n");
		exit(1);
	}

	exit (0);
}
