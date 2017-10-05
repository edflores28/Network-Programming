/*
 *	<Header stuff goes here>
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "unixnitslib.h"

int
main(int argc, char *argv[])
{
	int fd
	int bytes;
	int found;

	// Check to see if there are valid arguments.
	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <dest file>\n", argv[0]);
		exit (1);
	}

	// Obtain the socket file descriptor for the subscriber.
	// Exit is there is an error
	fd = setup_subscriber(PATH);

	if (fd == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "There was an error creating the subscriber.\n");
		exit(1);
	}

	printf("Sending %s to the publisher\n", argv[1]);

	found = access(article,F_OK);

	printf("found %i:", found);
	exit(1);

	// Send the Article that the subscriber wants from the publisher
	bytes = write(fd, "termcap", 8);
	//n = write(fd, "QUIT", 4);

	// Obtain a handle to write what we receive from the publisher.
	// Exit is there is an error.
	FILE *file;
	file = fopen("termcap", "w");

	if (file == NULL)
	{
		perror("ERROR");
		exit(1);
	}

	int buffer[256];

	// Read from the socket until there is is no more
	// data available from the subscriber. Also
	// write to the file.
	while(n != 0)
	{
		n = read(fd,buffer,255);
		fputs(buffer, file);
	}

	close(fd);
	fclose(file);
	exit (0);
}
