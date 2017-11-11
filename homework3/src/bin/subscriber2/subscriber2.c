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
#define DEFAULT_PORT	":7640"

#define BUFFER_SIZE 1024

/**
*	This method creates a datagram socket that will
* connect to the dicovery service which will request
* the available publishers.
*/
disc_pub_list request_list(char *host, char *port)
{
	char buffer[BUFFER_SIZE];
	disc_get_pub_list mesg;
	disc_pub_list list;
	int nbytes;
	socklen_t size;
	fd_set read;
	struct timeval time;
	int res;

	// Fill in the message structure.
	mesg.msg_type = 'G';

	socklen_t addrlen;
  struct sockaddr *addr;

	int fd = client_setup_sock(host, port, SOCK_DGRAM, &addrlen, &addr);
	if (fd == NITS_SOCKET_ERROR)
		exit(1);

	// Send the message to the discovery service.
	nbytes = sendto(fd, "G", 1, 0, addr, addrlen);

	printf("bytes sent: %d\n", nbytes);

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

	nbytes = recvfrom(fd, buffer, BUFFER_SIZE, 0, &addr, &addrlen);

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

/*
 * Usage: subscriber2 -h -d <discoveryhost>:<discoveryport>
 */

int main(int argc, char *argv[])
{
	// int c;
	// char *host;
	// char *port;
	// char *discovery = NULL;
	//
	// while ( (c = getopt(argc, argv, "hd:")) != -1)
	// {
	// 	switch (c)
	// 	{
	// 	    case 'd':
	// 		discovery = optarg;
	// 		break;
	// 	    case 'h':
	// 	    default:
	// 		fprintf (stderr, "Usage: %s -d <host:port>\n", argv[0]);
	// 		exit (1);
	// 	}
	// }
	// if (discovery == NULL)
	// {
	// 	discovery = malloc(MAXLEN);
	// 	strcpy (discovery, DEFAULT_PORT);
	// }
	//
	// get_host_and_port (discovery, &host, &port);
	//
	// /*
	//  * Create connectionless socket for discovery service, according to the
	//  * given command line parameters.
	//  */
	// printf ("Host: %s, Port: %s\n", host, port);
	int i, fd, bytes;
	char buffer[1024];
	disc_pub_list pub_list;

	pub_list = request_list("localhost", "8000");

	printf("%s\n", pub_list.num_publishers);
	exit(1);

	fd = setup_subscriber ("localhost", "8505");
	if (fd == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "Cannot set up publisher.\n");
		exit(1);
	}
	memset(&buffer[0],0,sizeof(buffer));
	bytes = read(fd, buffer, 1024);
	printf("bytes %i\n", bytes);

	exit (0);
}
