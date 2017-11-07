/*
 *	<Header stuff goes here>
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "nitslib.h"
#define MAXLEN 1024
#define SUB_BUFLEN 80
#define ART_BUFLEN 1024
#define ARTICLE_ROOT "/home/jcn/474/Articles"
#define DEFAULT_PUBLISHER "localhost:7640"
#define DEFAULT_DISCOVERY "localhost:7640"

static int Verbose = 0;

int
main(int argc, char *argv[])
{
	int c;
	char publisher[MAXLEN];
	char discovery[MAXLEN];
	char *phost, *pport;
	char *dhost, *dport;
	/*
	 * check args.
	 */
	strncpy (publisher, DEFAULT_PUBLISHER, MAXLEN);
	strncpy (discovery, DEFAULT_DISCOVERY, MAXLEN);

	while ( (c = getopt(argc, argv, "hvp:d:")) != -1)
	{
		switch (c)
		{
		    case 'v':
			Verbose = 1;
			break;
		    case 'p':
			strncpy (publisher, optarg, MAXLEN);
			break;
		    case 'd':
			strncpy (discovery, optarg, MAXLEN);
			break;
		    case 'h':
		    default:
			fprintf (stderr, "Usage: %s -d <host:port> -p <host:port>\n", argv[0]);
			exit (1);
		}
	}

	if (Verbose)
	{
		printf ("publisher: %s\n", publisher);
		printf ("discovery: %s\n", discovery);
	}

	get_host_and_port (publisher, &phost, &pport);
	
	if ((setup_publisher (phost, pport)) == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "Cannot set up publisher.\n");
		exit(1);
	}
	exit (0);
}
