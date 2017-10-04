#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unixnitslib.h"

// Global Variables
static int listen_fd;

int setup_subscriber(char *publisher_path)
{
	int sub_fd;
	int result;
	int i;
	struct sockaddr_un client_addr;

	// Obtain a file descriptor for the server.
	sub_fd = socket(AF_LOCAL, SOCK_STREAM, 0);

	if (sub_fd == -1)
	{
		perror("Error creating socket");
		return NITS_SOCKET_ERROR;
	}

	// Clear out server_addr so it can be used.
	memset(&client_addr, 0, sizeof(client_addr));

	// Copy the publisher path to the server_addr path and set
	// the socket family to local.
	strncpy(client_addr.sun_path, publisher_path, sizeof(client_addr.sun_path) - 1);
	client_addr.sun_family = AF_LOCAL;

	// Loop 6 times until a connection can be established. There
	// will be a 0.5 second delay after a failure is determined.
	for (i = 0; i < 6; i++)
	{
		result = connect(sub_fd, (struct sockaddr*) &client_addr, sizeof(client_addr));
		if (result == 0)
			break;
		else
		{
			perror("Error connecting");
			sleep(1);
		}
	}

	// If the counter is set to 6 return an error.
	if (i == 6)
		return NITS_SOCKET_ERROR;

	// Return the file descriptor.
	printf ("Setting up unix domain subscriber on %s\n", publisher_path);
	return sub_fd;
}

int setup_publisher(char *publisher_path)
{
	int result;
	struct sockaddr_un server_addr;

	// Obtain a file descriptor for the server.
	listen_fd = socket(AF_LOCAL, SOCK_STREAM, 0);

	if (listen_fd == -1)
	{
		perror("Error creating socket");
		return NITS_SOCKET_ERROR;
	}

	// Clear out server_addr so it can be used.
	memset(&server_addr, 0, sizeof(server_addr));

	// Unlink any instances.
	unlink(publisher_path);

	// Copy the publisher path to the server_addr path and set
	// the socket family to local.
	strncpy(server_addr.sun_path, publisher_path, sizeof(server_addr.sun_path) - 1);
	server_addr.sun_family = AF_LOCAL;

	// Bind to the socket
	result = bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

	if (result == -1)
	{
		perror("Error binding");
		return NITS_SOCKET_ERROR;
	}

	// Listem on the socket
	result = listen(listen_fd, 5);

	if (result == -1)
	{
		perror("Error listening");
		return NITS_SOCKET_ERROR;
	}

	printf ("Setting up unix domain publisher eererer on %s\n", publisher_path);
	return (NITS_SOCKET_OK);
}

int get_next_subscriber(void)
{
	struct sockaddr_un cli_addr;
	int sock_fd, addr_len;

	if (!(listen_fd > 0))
	{
		printf("The publisher has not been set up!\n");
		return NITS_SOCKET_ERROR;
	}

	addr_len = sizeof(cli_addr);

	sock_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &addr_len);
	
	if (sock_fd == -1)
	{
		perror("Error accepting");
		return NITS_SOCKET_ERROR;
	}

	return sock_fd;
}
