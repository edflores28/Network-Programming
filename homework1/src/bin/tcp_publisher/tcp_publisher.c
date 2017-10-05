/*
 *	<Header stuff goes here>
 */
#include <stdlib.h>
#include <stdio.h>
#include "tcpnitslib.h"
#define MY_PORT	7640

int
main(int argc, char *argv[])
{
	int pub_fd;
	/*
	 * check args.
	 */
	if (argc > 1)
	{
		fprintf (stderr, "Usage: %s\n", argv[0]);
		exit (1);
	}
	/*
	 * do your stuff here...just calling setup_publisher
	 * in this example for fun.
	 */
	if ((pub_fd = setup_publisher (MY_PORT)) == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "Stupid idiot, you screwed up. You know nothing.\n");
		exit(1);
	}

	exit (0);
}
