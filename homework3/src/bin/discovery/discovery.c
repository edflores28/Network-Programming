/*
 *Project: Assignment 3
 *
 *Progam: discovery
 *File Name: discovery.c
 *Purpose: Service that communicates with both the
 *         publisher and subscriber via datagrams.
 *
 *Programmer: Edwin Flores
 *Course: EN.605.474.81
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "nitslib.h"
#include "config.h"

#define MAXLEN (128)
#define BUFFER_SIZE 1024

/**
*	The main program.
*/
int main(int argc, char *argv[])
{
	// Variables
	int fd, c, recvlen, nbytes, total_pubs = 0;
	char buffer[BUFFER_SIZE];
	DISCOVERY_MESSAGES msg_type;
	disc_get_pub_list sub_mesg;
	disc_advertise pub_mesg;
	disc_pub_list disc_mesg;
	ADDRESS publisher_address[NUM_PUBLISHERS];
	struct sockaddr_storage addr;
	socklen_t size;
	char *host, *port;
	char discovery[MAXLEN];

	// Copy the default discovery path.
	strcpy (discovery, DEFAULT_DISCOVERY);

	// Check and parse if there are and command line options.
	while ( (c = getopt(argc, argv, "d:")) != -1)
	{
		switch (c)
		{
				case 'd':
					strncpy (discovery, optarg, MAXLEN);
					break;
				default:
					fprintf (stderr, "Usage: %s -d <host:port>\n", argv[0]);
					exit (1);
		}
	}

	// Parse the discovery information.
	get_host_and_port (discovery, &host, &port);

	// Create the socket.
	fd = setup_discovery(host, port);

	if (fd == NITS_SOCKET_ERROR)
	{
			perror("Error creating discovery service..exiting..");
			exit(1);
	}

	// Wait for messages.
	while(1)
	{
		printf("\nWaiting for messages!\n");

		// Clear the buffer
		memset(&buffer, 0, sizeof(buffer));
		memset(&msg_type, 0, sizeof(msg_type));

		size = sizeof(addr);

		// Read available data
		nbytes = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addr, &size);

		if (nbytes >= sizeof(msg_type))
			memcpy(&msg_type, &buffer, sizeof(msg_type));

		// Process messages send from the publisher
		if (msg_type == 'A')
		{
			printf("Message received from the publisher.\n");

			// Copy the buffer into the pub_mesg variable
			memcpy(&pub_mesg, &buffer, sizeof(pub_mesg));

			// If the publisher advertises it's address
			// store it into the list and increment the counter
			if (total_pubs < NUM_PUBLISHERS)
			{
				printf("Received ADVERTISE\n");

				memcpy(&publisher_address[total_pubs], &pub_mesg.pub_address, ADDRESS_LENGTH);
				total_pubs++;

				memset(&pub_mesg, 0, sizeof(pub_mesg));
			}
		}

		// Process messages sent from the subscriber.
		if (msg_type == 'G')
		{
			printf("Message received from the subsciber.\n");
			printf("Received GET_PUB_LIST\n");

			// Copy the buffer into the sub_mesg variables
			memcpy(&sub_mesg, &buffer, sizeof(sub_mesg));

			// Send the publisher addresses to the subscriber
			// if it was requested. Build the message.

			disc_mesg.msg_type = 'P';
			sprintf(disc_mesg.num_publishers, "%d", total_pubs);
			memcpy(&disc_mesg.publisher_address[0], &publisher_address, sizeof(disc_mesg.publisher_address));

			printf("Sending PUB_LIST\n");
			nbytes = sendto(fd, &disc_mesg, sizeof(disc_mesg), 0, (struct sockaddr*)&addr, size);

			if (nbytes < 0)
				perror("Error Sending");

			// Clear the message
			memset(&sub_mesg, 0, sizeof(sub_mesg));
		}
	}
}
