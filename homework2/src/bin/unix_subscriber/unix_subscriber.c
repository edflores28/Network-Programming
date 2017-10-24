/*
 *Project: Assignment 1
 *
 *Progam: tcp_subscriber
 *File Name: tcp_subscriber.c
 *Purpose: Requests articles from the publisher over
 *         a UNIX socket.
 *
 *Programmer: Edwin Flores
 *Course: EN.605.474.81
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include "unixnitslib.h"

#define BUFFER_SIZE 1024

disc_pub_list request_list()
{
	struct sockaddr_un server;
	char tmp[L_tmpnam], buffer[BUFFER_SIZE];
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
	tmpnam(tmp);
	fd = setup_discovery_server(tmp);

	// Set the discovery service information.
	server.sun_family = AF_LOCAL;
	strncpy(server.sun_path, DISCOVERY_PATH, sizeof(server.sun_path) - 1);

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

int main(int argc, char *argv[])
{
	// Variables
	int fd;
	int bytes;
	int found, res;
	char buffer[BUFFER_SIZE];
	int init_read = -1;
	int length, user, i;
	int list = -1;
	char article[128];
	FILE *file;
	disc_pub_list pub_list;
	fd_set timeout;
	struct timeval time;

	// Check to see if there are valid arguments.
	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <dest file>\n", argv[0]);
		exit (1);
	}

	pub_list = request_list();

	printf("The following is a list of available publishers:\n");
	
	for (i = 0; i < pub_list.num_publishers; i++)
		printf("%i:\t%s\n", i, pub_list.address[i].sun_path);

	printf("Select a publisher (0 - %i): ", i-1);
	scanf("%i", &user);

	// Obtain the socket file descriptor for the subscriber based
	// on the user's input. Exit is there is an error
	fd = setup_subscriber((char *)pub_list.address[user].sun_path);
	
	if (fd == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "There was an error creating the subscriber.\n");
		exit(1);
	}

	// Obtain the list of articles
	printf("\nObtaining the list of articles from the publisher...\n\n");
	bytes = write(fd,"LIST",4);

	printf("bytes: %i\n", bytes);

	// Obtain the LIST from the publisher. Assuming that the publisher will
	// only send the buffer size of data.
	bytes = read(fd,buffer,BUFFER_SIZE);

	printf("The following articles are available:\n");
	printf("%s\n",buffer);

	memset(&article, 0, sizeof(article));

	printf("Enter the name of the article: ");
	scanf("%s",article);
	printf("\n");

	for (i = 0; i < BUFFER_SIZE; i++)
	{
		if (article[i] == 0)
			break;

		length++;
	}

	printf("Requesting %s\n",article);
	bytes = write(fd, article, length);
 	
	printf("BYTES %i\n", bytes);

	time.tv_sec = 0;
	time.tv_usec = 100000;
	
	FD_ZERO(&timeout);
	FD_SET(fd, &timeout);

	// Read from the socket until there is is no more
	// data available from the subscriber. Also
	// write to the file.
	while(1)
	{
		printf("inside while:%i\n",bytes);
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
		}

		// Write to the file.
		fputs(buffer, file);

		// Clear the buffer.
		memset(&buffer, 0, sizeof(buffer));
	}

	printf("ehrere\n");
	// Print out a message if there were no bytes reads.
	if (init_read == -1)
		printf("There was nothing recieved from the publisher\n");

	// Do some cleanup.
	close(fd);
	fclose(file);
	exit (0);
}
