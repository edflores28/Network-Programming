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
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "tcpnitslib.h"
#include "config.h"

#define ARRAY_SIZE 1024

/**
*	This method creates a datagram socket that will
* connect to the dicovery service which will request
* the available publishers.
*/
disc_pub_list request_list()
{
	struct sockaddr_in server;
	char buffer[ARRAY_SIZE];
	disc_get_pub_list mesg;
	disc_pub_list list;
	int nbytes, fd;
	socklen_t size;
	fd_set read;
	struct timeval time;
	int res;

	// Fill in the message structure.
	mesg.msg_type = GET_PUB_LIST;

	// Create the client.
	fd = setup_discovery_server(0);

	// Set the discovery service information.
	server.sin_family = AF_INET;
	server.sin_port = htons(UDP_PORT);

	if (inet_aton("128.220.101.247",&server.sin_addr)==0)
	{
		perror("inet_aton error..exit..\n");
		exit(1);
	}

	// Send the message to the discovery service.
	nbytes = sendto(fd, &mesg, sizeof(mesg), 0, (struct sockaddr *)&server, sizeof(server));

	// Implement a 5 second wait.
	if (nbytes < 0)
	{
		perror("Error sending");
		exit(1);
	}

	// Set up the timer for select
	time.tv_sec = 5;
	time.tv_usec = 0;

	// Add the file descriptor to the set.
	FD_ZERO(&read);
	FD_SET(fd, &read);

	res = select(fd+1, &read, NULL, NULL, &time);

	// Print out error message and exit.
	if (res < 0)
	{
		perror("Select error or timeout reached\n");
		close(fd);
		exit(1);
	}

	// Print timeout reached and exit.
	if (res == 0)
	{
		printf("Timeout reached. No messages received...exiting..\n");
		close(fd);
		exit(1);
	}

	nbytes = recvfrom(fd, buffer, ARRAY_SIZE, 0, (struct sockaddr*)&server, &size);

	if (nbytes < 0)
	{
		perror("Error reading\n");
	}

	printf("bytes: %i\n", nbytes, sizeof(disc_pub_list));
	if (nbytes == sizeof(disc_pub_list))
	{
		printf("Publisher list received from discovery service\n");
		memset(&list, 0, sizeof(list));
		memcpy(&list, &buffer, sizeof(list));

		if (list.num_publishers == 0)
		{
			printf("There are no publishers available..exiting.\n");
			close(fd);
			exit(1);
		}
	}
	close(fd);
	return list;
}

/**
*	The main program.
*/
int main(int argc, char *argv[])
{
	// Variables
	int fd;
	int bytes, i;
	int found, user;
	char buffer[ARRAY_SIZE];
	int init_read = -1;
	int length;
	struct in_addr *host;
	int list = -1;
	FILE *file;
	disc_pub_list pub_list;

	// Check to see if there are valid arguments.
	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <article> [opt IP addr]\n", argv[0]);
		exit (1);
	}

	// Request the the available publishers from the
	// discovery service.
	pub_list = request_list();

	printf("The following is a list of available publishers:\n");

	for (i = 0; i < pub_list.num_publishers; i++)
		printf("%i:\t%s\n", i+1, pub_list.address[i].sin_addr);

	printf("Enter 6 to QUIT or -\n");
	printf("Select a publisher (1 - %i): ", i);
	scanf("%i", &user);

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
		if ((init_read == -1) && (list != 0))
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
	if ((init_read == -1) && (list != 0))
		printf("There was nothing recieved from the publisher\n");

	// Do some cleanup.
	close(fd);
	fclose(file);
	exit (0);
}
