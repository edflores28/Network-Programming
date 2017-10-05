/*
 *	<Header stuff goes here>
 */
#include <stdlib.h>
#include <stdio.h>
#include "unixnitslib.h"

int
main(int argc, char *argv[])
{
	int fd, n;
	/*
	 * check args.
	 */
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
		fprintf (stderr, "Stupid idiot, you screwed up. You know nothing.\n");
		exit(1);
	}

	// Send the Article that the subscriber wants from the publisher
	n = write(fd, "termcap", 8);
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
