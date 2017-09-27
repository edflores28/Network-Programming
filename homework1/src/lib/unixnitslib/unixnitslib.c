#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include "unixnitslib.h"

// Global Variables
static int server_fd;

int setup_subscriber(char *publisher_path)
{
	printf ("Setting up unix domain subscriber on %s\n", publisher_path);
	return (0);
}

int setup_publisher(char *publisher_path)
{
	int result;
	struct sockaddr_un server_addr;

	// Obtain a file descriptor for the server.
	server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);

	if (server_fd == -1)
	{
		perror("Error creating socket");
		return NITS_SOCKET_ERROR;
	}

	// Clear out server_addr so it can be used.
	memset(&server_addr, 0, sizeof(server_addr));

	// Copy the publisher path to the server_addr path.
	strncpy(server_addr.sun_path, publisher_path, sizeof(server_addr.sun_path) - 1);

	// Unlink any instances.
	unlink(publisher_path);

	server_addr.sun_family = AF_LOCAL;

	// Bind to the socket
	result = bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	
	if (result == -1)
	{
		perror("Error binding");
		return NITS_SOCKET_ERROR;
	}

	printf ("Setting up unix domain publisher eererer on %s\n", publisher_path);
	return (NITS_SOCKET_OK);
}
