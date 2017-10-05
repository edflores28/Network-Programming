/*
 *	<Header stuff goes here>
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "tcpnitslib.h"

#define MY_PORT	7640

int main(int argc, char *argv[])
{
	int fd;
	int bytes;
	int found;
	char buffer[256];
	int init_read = -1;
	int length;
	struct in_addr *host;
	/*
	 * make sure to fill in the host information correctly.
	 */
	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <article> [opt IP addr]\n", argv[0]);
		exit (1);
	}
	/*
	 * do your stuff here...just calling setup_subscriber
	 * in this example for fun.
	 */
	fd = setup_subscriber (host, MY_PORT);

	if (fd == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "There was an error creating the subscriber.\n");
		exit(1);
	}

	printf("Sending %s to the publisher\n", argv[1]);

	// Send the Article that the subscriber wants from the publisher
	// First make sure we get the size of the file.
	for (length = 0; length < 25; length++)
		if (argv[1][length] == 0x00)
			break;

	bytes = write(fd,argv[1],length);

	// Obtain a handle to write what we receive from the publisher.
	// Exit is there is an error.
	FILE *file;
	file = fopen(argv[1], "wb");

	if (file == NULL)
	{
		perror("Unable to open and write the file");
		close(fd);
		exit(1);
	}

	// Read from the socket until there is is no more
	// data available from the subscriber. Also
	// write to the file.
	while(bytes != 0)
	{
		bytes = read(fd,buffer,255);

		// If nothing comes from the buffer in the initial read
		// break from the loop, otherwhise do not break.
		if (init_read == -1)
			if (bytes == 0)
				break;
			else
				init_read = 0;

		fputs(buffer, file);
	}

	if (init_read == -1)
		printf("There was nothing recieved from the publisher\n");

	close(fd);
	fclose(file);
	exit (0);
}
