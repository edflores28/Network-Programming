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

	if ((setup_publisher (phost, pport)) == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "Cannot set up publisher.\n");
		exit(1);
	}
	exit (0);
}
