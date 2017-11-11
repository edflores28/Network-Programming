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

	register_publisher("localhost", "8505", "localhost", "8000");
	
	if ((setup_publisher ("localhost", "8505")) == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "Cannot set up publisher.\n");
		exit(1);
	}

	fd = get_next_subscriber();

printf("GOT IT\n");
	if (fd == NITS_SOCKET_ERROR)
	{
		printf("ERROR SETUP");
		exit(1);
	}

	int bytes = write (fd, "Hello", 5);
	printf("bytes: %d\n", bytes);
	scanf("%i",&c);
	exit (0);
}
