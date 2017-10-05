/*
 * Pretty header file stuff is here.
 */
#include <arpa/inet.>
#include <stdio.h>
#include "tcpnitslib.h"

// Global Variables
static int listen_fd;

int setup_subscriber(struct in_addr *host, int port)
{
	int sub_fd;
	int result;
	int i;
	struct sockaddr_in client_addr;

	// Obtain a file descriptor for the server.
	sub_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (sub_fd == -1)
	{
		perror("Error creating socket");
		return NITS_SOCKET_ERROR;
	}

	// Clear out server_addr so it can be used.
	memset(&client_addr, 0, sizeof(client_addr));

	// Set the TCP configuration
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(port);
	client_addr.sin_addr.sddr = host;

	// Loop 6 times until a connection can be established. There
	// will be a 1 second delay after a failure is determined.
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
	printf ("Setting up tcp subscriber on %d\n", port);
	return sub_fd;
}

int setup_publisher(int port)
{
	printf ("Setting up tcp publisher on %d\n", port);

	int result;
	struct sockaddr_in server_addr;

	// Obtain a file descriptor for the server.
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (listen_fd == -1)
	{
		perror("Error creating socket");
		return NITS_SOCKET_ERROR;
	}

	// Clear out server_addr so it can be used.
	memset(&server_addr, 0, sizeof(server_addr));

	// Unlink any instances.
	unlink(publisher_path);

	// Set the family and the port number.
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port)

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

	printf ("Setting up tcp publisher on %d\n", port);
	return (NITS_SOCKET_OK);
}

int get_next_subscriber(void)
{
	struct sockaddr_in cli_addr;
	int sock_fd, addr_len;

	// Return the error if the socket has not been created
	if (!(listen_fd > 0))
	{
		printf("The publisher has not been set up!\n");
		return NITS_SOCKET_ERROR;
	}

	// Accept the subscriber on obtain a socket.
	addr_len = sizeof(cli_addr);
	sock_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &addr_len);

	// Return the error if the socket has not been created
	if (sock_fd == -1)
	{
		perror("Error accepting");
		return NITS_SOCKET_ERROR;
	}

	return sock_fd;
}
