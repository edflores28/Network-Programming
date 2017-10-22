/*
 *Project: Assignment 1
 *
 *Progam: tcp_subscriber
 *File Name: tcp_subscriber.c
 *Purpose: Requests articles from the publisher over
 *         a TCP socket.
 *
 *Programmer: Edwin Flores
 *Course: EN.605.474.81
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "tcpnitslib.h"
#include "config.h"

#define ARRAY_SIZE 1024

int main(int argc, char *argv[])
{
	// Variables
	int fd;
	int bytes;
	int found;
	char buffer[ARRAY_SIZE];
	int init_read = -1;
	int length;
	struct in_addr *host;
	int list = -1;
	FILE *file;

	// Check to see if there are valid arguments.
	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <article> [opt IP addr]\n", argv[0]);
		exit (1);
	}

	// Set the host to be localhost
	host = (uint32_t) inet_addr("127.0.0.1");

	// If the user entered an ip address use it as
	// the host.
	if (argc > 2)
		host = (uint32_t) inet_addr(argv[2]);

	// Obtain the socket file descriptor for the subscriber.
	// Exit is there is an error
	fd = setup_subscriber (host, TCP_PORT);

	if (fd == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "There was an error creating the subscriber.\n");
		exit(1);
	}

	// Determine if the user wants the list of articles.
	list = strcmp("LIST", argv[1]);

	printf("Sending %s to the publisher\n", argv[1]);

	// Send the Article that the subscriber wants from the publisher
	// First make sure we get the size of the file.
	for (length = 0; length < 64; length++)
		if (argv[1][length] == 0x00)
			break;

	bytes = write(fd,argv[1],length);

	if (list == 0)
		printf("\nThe following files are available to be requested:\n\n");

	// Read from the socket until there is is no more
	// data available from the subscriber. Also
	// write to the file.
	while(1)
	{
		bytes = read(fd,buffer,ARRAY_SIZE);

		// If there are no bytes read break from the while loop.
		if (bytes == 0)
			break;

		// At this point bytes are read and if it is the initial
		// loop open the file to write.
		if (init_read == -1)
		{
			init_read = 0;

			// Obtain a handle to write what we receive from the publisher.
			// Exit is there is an error.
			file = fopen(argv[1], "wb");

			if (file == NULL)
			{
				perror("Unable to open and write the file");
				close(fd);
				exit(1);
			}
		}

		// If LIST was send print the buffer, otherwise write
		// to the file.
		if (list == 0)
			printf("%s",buffer);
		else
			fputs(buffer, file);

		// Clear the buffer.
		memset(&buffer, 0, sizeof(buffer));
	}

	// Print out a message if there were no bytes reads.
	if ((init_read == -1) && (list !=0))
		printf("There was nothing recieved from the publisher\n");

	// Do some cleanup.
	close(fd);
	fclose(file);
	exit (0);
}
