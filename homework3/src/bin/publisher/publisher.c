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

static int Verbose = 0;

int
main(int argc, char *argv[])
{
	int c;
	char publisher[MAXLEN];
	char discovery[MAXLEN];
	char *phost, *pport;
	char *dhost, *dport;
	int fd;
	if ((setup_publisher ("/unix", "/tmp/hello")) == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "Cannot set up publisher.\n");
		exit(1);
	}

	fd = get_next_subscriber();

	if (fd == NITS_SOCKET_ERROR)
		exit(1);
	printf("success");
	exit (0);
}
