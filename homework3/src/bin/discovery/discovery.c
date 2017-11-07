/*
 *	Discovery service
 *
 * Usage : discovery -d <host>:<port>
 *	   discovery -d :<port>
 * If host not supplied, use ""
 * If port not supplied, use DEFAULT_DISCOVERY (:<your port>)
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "nitslib.h"

#define MAXLEN (128)
#define DEFAULT_DISCOVERY ":7640"

int
main(int argc, char *argv[])
{
	char *host, *port;
	char discovery[MAXLEN];
	int discovery_fd;
	int c;

	/*
	 * Make sure discovery is pointing to a writable string.
	 * That's why we 
	 */
	strcpy (discovery, DEFAULT_DISCOVERY);

	/*
	 * check args.
	 */
	while ( (c = getopt(argc, argv, "d:h")) != -1)
	{
		switch (c)
		{
		    case 'h':
			fprintf (stderr, "Usage: %s -d <host:port>\n", argv[0]);
		    	exit (0);
		    case 'd':
			strncpy (discovery, optarg, MAXLEN);
			break;
		    default:
			fprintf (stderr, "Usage: %s -d <host:port>\n", argv[0]);
			exit (1);
		}
	}

        get_host_and_port (discovery, &host, &port);

	if ((discovery_fd = setup_discovery (host, port)) == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "Setup discovery failed.\n");
		exit(1);
	}

	exit (0);
}
