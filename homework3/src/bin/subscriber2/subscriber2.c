/*
 *Project: Assignment 3
 *
 *Progam: subscriber2
 *File Name: subscriber2.c
 *Purpose: Communicates with publisher and discovery services.
 *         to obtain an article. Protocol independent implementation
 *
 *Programmer: Edwin Flores
 *Course: EN.605.474.81
 *
 */
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
#define DEFAULT_PORT	"localhost:8404"

#define BUFFER_SIZE 1024

/**
*	This method creates a datagram socket that will
* connect to the dicovery service which will request
* the available publishers.
*/
disc_pub_list request_list(char *host, char *port)
{
	// Variables.
	char buffer[BUFFER_SIZE];
	disc_get_pub_list mesg;
	disc_pub_list list;
	int nbytes, result, status;
	socklen_t size;
	fd_set recv;
	struct timeval time;
	struct addrinfo hints, *res, *ptr;
	struct sockaddr addr_cpy;
	int fd = -1, val = 1;

	// Zero out the hints and set the hints.
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	// Obtain the list of addresses
	if ((status = getaddrinfo(host, port, &hints, &res)) != 0)
	{
		perror("getaddrinfo error");
		printf("%s\n", gai_strerror(status));
		exit(1);
	}

	// If the address is AF_LOCAL/UNIX then call setup discovery.
	// This will create the end point that is needed in order
	// to communicate properly. Otherwise just create a socket.
	if (res->ai_family == AF_UNIX)
		fd = setup_discovery(host, NULL);
	else
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// Exit if there was an issue.
	if (fd == NITS_SOCKET_ERROR)
		exit(1);

	// Establish a connection.
	if (connect(fd, res->ai_addr, res->ai_addrlen) < 0)
  {
		perror("connect error");
		exit(1);
	}

	// Fill in the message structure.
	mesg.msg_type = 'G';

	// Send the message to the discovery service.
	nbytes = write(fd, &mesg, sizeof(mesg));

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
	FD_ZERO(&recv);
	FD_SET(fd, &recv);

	result = select(fd+1, &recv, NULL, NULL, &time);

	// Print out error message and exit.
	if (result < 0)
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

	// Obtain the discovery service data.
	nbytes = read(fd, buffer, BUFFER_SIZE);
	if (nbytes < 0)
	{
		perror("Error reading\n");
	}

	// Check to see if we recieved the right sized message.
	if (nbytes == sizeof(disc_pub_list))
	{
		printf("Publisher list received from discovery service\n");
		memset(&list, 0, sizeof(list));
		memcpy(&list, &buffer, sizeof(list));
	}

	freeaddrinfo(res);
	close(fd);
	return list;
}

/**
*	The main program.
*/
int main(int argc, char *argv[])
{
	// Variables
	int i, fd, bytes, num_pubs, c;
	int user, length, init_read = -1;
	char buffer[BUFFER_SIZE];
	char *pub_host;
	char *pub_port;
	disc_pub_list pub_list;
	fd_set timeout;
	struct timeval time;
	char article[MAXLEN];
	FILE *file;
	char *host;
	char *port;
	char *discovery = NULL;

	// Check and parse if there are and command line options.
	while ((c = getopt(argc, argv, "d:")) != -1)
	{
		switch (c)
		{
				case 'd':
				discovery = optarg;
				break;
				default:
				fprintf (stderr, "Usage: %s -d <host:port>\n", argv[0]);
				exit (1);
		}
	}

	// Set the discovery information if nothing was entered, and
	// split the string.
	if (discovery == NULL)
	{
		discovery = malloc(MAXLEN);
		strcpy (discovery, DEFAULT_PORT);
	}

	get_host_and_port (discovery, &host, &port);

	// Request a list of publishers.
	pub_list = request_list(host, port);
	num_pubs = atoi(pub_list.num_publishers);

	if (num_pubs == 0)
	{
		printf("There are no publishers available..exiting.\n");
		exit(1);
	}

	// Request the the available publishers from the
	// discovery service.
	printf("The following is a list of available publishers:\n");

	for (i = 0; i < num_pubs; i++)
		printf("%i:\t%s\n", i+1, pub_list.publisher_address[i]);

	printf("Enter 6 to QUIT or -\n");
	printf("Select a publisher (1 - %i): ", i);
	scanf("%i", &user);

	if (user == 6)
		exit(0);

	// Decrement the user's input and see if the
	// selection is within range. If the value is not
	// within range, exit.
	user--;
	if ((user >= i )&& (user < 0))
	{
		printf("Invalid selection. exiting.");
		exit(0);
	}

	// Parse the publisher port and host information.
	get_host_and_port(pub_list.publisher_address[user], &pub_host, &pub_port);

	// Obtain the socket file descriptor for the subscriber.
	// Exit is there is an error
	fd = setup_subscriber (pub_host, pub_port);

	if (fd == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "There was an error creating the subscriber.\n");
		exit(1);
	}

	// Set up the select system call to sleep
	// for 100ms. This will help to not block
	// on the read call when we need to exit.
	time.tv_sec = 0;
	time.tv_usec = 100000;

	FD_ZERO(&timeout);
	FD_SET(fd, &timeout);

	// Obtain the list of articles
	printf("\nObtaining the list of articles from the publisher...\n\n");
	bytes = write(fd,"LIST",4);

	if (bytes < 0)
	{
		perror("Error writing\n");
		close(fd);
		exit(1);
	}

	// Obtain the LIST from the publisher. Assuming that the publisher will
	// only send the buffer size of data.
	bytes = read(fd,buffer,BUFFER_SIZE);

	if (bytes < 0)
	{
		perror("Error reading\n");
		close(fd);
		exit(1);
	}

	// Ask the user's input on which publisher that they want to use.
	printf("The following articles are available:\n");
	printf("%s\n",buffer);

	memset(&article, 0, sizeof(article));

	printf("Enter the name of the article: ");
	scanf("%s",article);
	printf("\n");

	// Obtain the length of the user's input.
	for (i = 0; i < BUFFER_SIZE; i++)
	{
		if (article[i] == 0)
			break;

			length++;
	}

	printf("Requesting %s\n",article);
	bytes = write(fd, article, length);

	if (bytes < 0)
	{
		perror("Error writing");
		exit(1);
	}

	// Read from the socket until there is is no more
	// data from the publisher. Once the timeout of
	// 100ms is reached we will break from the loop.
	while(1)
	{

		if ((select(fd+1, &timeout, NULL, NULL, &time)) == 0)
			break;

		bytes = read(fd,buffer,BUFFER_SIZE);

		// At this point bytes are read and if it is the initial
		// loop open the file to write.
		if (init_read == -1)
		{
			init_read = 0;

			// Obtain a handle to write what we receive from the publisher.
			// Exit is there is an error.
			file = fopen(article, "wb");

			if (file == NULL)
			{
				perror("Unable to open and write the file");
				close(fd);
				exit(1);
			}

			printf("Opened the file for writing.\n");
		}

		// Write to the file.
		fputs(buffer, file);

		// Clear the buffer.
		memset(&buffer, 0, sizeof(buffer));
	}

	printf("Finished writing to the file\n");

	// Print out a message if there were no bytes reads.
	if (init_read == -1)
		printf("There was nothing recieved from the publisher\n");

	printf("Enter QUIT\n");
	printf("This will interrupt other clients as well: ");
	scanf("%s",article);

	// Send QUIT to kill the publisher.
	bytes = write(fd, "QUIT", 4);

	// Do some cleanup.
	close(fd);
	fclose(file);
	exit (0);
}
