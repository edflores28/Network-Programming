/*
 *	<Header stuff goes here>
 */
#include <stdlib.h>
#include <stdio.h>
#include "unixnitslib.h"

int
main(int argc, char *argv[])
{
	/*
	 * check args.
	 */
	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <source file>\n", argv[0]);
		exit (1);
	}
	/*
	 * do your stuff here...just calling setup_publisher
	 * in this example for fun.
	 */
	if (setup_publisher ("./serversocket") == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "Stupid idiot, you screwed up.\n");
		exit(1);
	}

	exit (0);
}
