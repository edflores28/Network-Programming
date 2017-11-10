#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include "nitslib.h"

#define MAXLEN	256
#define DEFAULT_PORT	":7640"

/*
 * Usage: subscriber2 -h -d <discoveryhost>:<discoveryport>
 */

int main(int argc, char *argv[])
{
	// int c;
	// char *host;
	// char *port;
	// char *discovery = NULL;
	//
	// while ( (c = getopt(argc, argv, "hd:")) != -1)
	// {
	// 	switch (c)
	// 	{
	// 	    case 'd':
	// 		discovery = optarg;
	// 		break;
	// 	    case 'h':
	// 	    default:
	// 		fprintf (stderr, "Usage: %s -d <host:port>\n", argv[0]);
	// 		exit (1);
	// 	}
	// }
	// if (discovery == NULL)
	// {
	// 	discovery = malloc(MAXLEN);
	// 	strcpy (discovery, DEFAULT_PORT);
	// }
	//
	// get_host_and_port (discovery, &host, &port);
	//
	// /*
	//  * Create connectionless socket for discovery service, according to the
	//  * given command line parameters.
	//  */
	// printf ("Host: %s, Port: %s\n", host, port);
	int i;
	if ((setup_subscriber ("localhost", "8005")) == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "Cannot set up publisher.\n");
		exit(1);
	}
	scanf("%d\n",&i);
	exit (0);
}
