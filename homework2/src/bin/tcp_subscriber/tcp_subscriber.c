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
#include "utillib.h"

#define BUFFER_SIZE 1024

/**
*	This method creates a datagram socket that will
* connect to the dicovery service which will request
* the available publishers.
*/
disc_pub_list request_list(char *discover)
{
	struct sockaddr_in server;
	char buffer[BUFFER_SIZE];
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

	if (inet_aton(discover,&server.sin_addr)==0)
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

	nbytes = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server, &size);

	if (nbytes < 0)
	{
		perror("Error reading\n");
	}

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
	int fd, bytes, found, res;
	int length = 0;
	int user, i;
	char buffer[BUFFER_SIZE];
	int init_read = -1;
	int list = -1;
	char article[128];
	FILE *file;
	disc_pub_list pub_list;
	fd_set timeout;
	struct timeval time;
	char *disc_addr;

	disc_addr = parse_arg(argc, argv);

	if (disc_addr == NULL) {
		printf("I AM NULL")
		disc_addr = DEFAULT_TCP_DISC;
	}

	// Request the the available publishers from the
	// discovery service.
	pub_list = request_list(disc_addr);

	printf("The following is a list of available publishers:\n");

	for (i = 0; i < pub_list.num_publishers; i++)
		printf("%i:\t%s\n", i+1, inet_ntoa(pub_list.address[i].sin_addr));

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

	// Obtain the socket file descriptor for the subscriber.
	// Exit is there is an error
	fd = setup_subscriber (&pub_list.address[user].sin_addr, TCP_PORT);

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
		perror("Error writing\n");
		exit(1);
	}

	// Read from the socket until there is is no more
	// data from the publisher. Once the timeout of
	// 100ms is reached we will break from the loop.
	while(1)
	{
		res = select(fd+1, &timeout, NULL, NULL, &time);

		if (res == 0)
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
