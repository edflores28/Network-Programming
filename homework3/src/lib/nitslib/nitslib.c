/*
*Project: Assignment 3
*
*Library: nitslib
*File Name: nitslib.cp
*Purpose: provides utilities to set up client and server
*         sockets. Protocol independent version.
*
*Synopsis (Usage and Parameters):
*
*	  setup_subscriber(char *host, char *port)
*
* 	setup_publisher(cchar *host, char *port)
*
* 	get_next_subscriber(void)
*
*   setup_discovery(char *host, char *port)
*
*   register_publisher (char *host, char *port, char *dhost, char *dport);
*
*   get_host_and_port (char *hostport, char **host, char **port);
*
*Programmer: Edwin Flores
*Course: EN.605.474.81
*
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include "nitslib.h"
#include <arpa/inet.h>
#include <sys/un.h>

#define BUFFER_SIZE 1024

// Global Variables
static int listen_fd;

/*
* Function that sets up the subscriber and returns
* a socket.
*/
int setup_subscriber(char *host, char *port)
{
	// Variables
	int i, result, fd, status, val = 1;
	struct addrinfo hints, *res;
	struct sockaddr addr_cpy;
	char gethost[BUFFER_SIZE];
	char getserv[BUFFER_SIZE];

	printf ("Calling setup_subscriber %s %s\n", host, port);

  // Zero out the hints and set the hints.
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	//Obtain the list of addresses.
	if ((status = getaddrinfo(host, port, &hints, &res)) != 0)
	{
		perror("getaddrinfo error");
		printf("%s\n", gai_strerror(status));
		return NITS_SOCKET_ERROR;
	}

	// Create a socket with the information from getaddrinfo.
	fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (fd == -1)
	{
		perror("socket error");
		return NITS_SOCKET_ERROR;
	}

	// Loop 6 times until a connection can be established. There
	// will be a 1 second delay after a failure is determined.
	for (i = 0; i < 6; i++)
	{
		result = connect(fd, res->ai_addr, res->ai_addrlen);

		if (result == 0)
			break;
		else
		{
			perror("connect error.");
			sleep(1);
		}
	}

	// If the counter is set to 6 return an error.
	if (i == 6)
		return NITS_SOCKET_ERROR;

	// Obtain the host and port and print it out.
	getnameinfo(res->ai_addr, res->ai_addrlen, gethost, sizeof gethost, getserv, sizeof getserv, NI_NUMERICSERV);
	printf ("Subscriber created on %s %s\n\n", gethost, getserv);

	freeaddrinfo(res);
	return fd;
}

/*
* Function that sets up the subscriber and returns
* a socket
*/
int setup_publisher(char *host, char *port)
{
	// Variables
	struct addrinfo hints, *res, *ptr;
	int fd, status, val =1;
	char gethost[BUFFER_SIZE];
	char getserv[BUFFER_SIZE];

  printf ("Calling setup_publisher %s %s\n", host, port);

 // Zero out the hints and set the hints.
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// Obtain the list of addresses
	if ((status = getaddrinfo(host, port, &hints, &res)) != 0)
	{
		perror("getaddrinfo error");
		printf("status: %d", status);
		printf("%s\n", gai_strerror(status));
		return NITS_SOCKET_ERROR;
	}

	// Create a socket with the information from getaddrinfo.
	fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (fd == -1)
	{
		perror("socket error");
		return NITS_SOCKET_ERROR;
	}

	// Enable SO_REUSEADDR.
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
	{
		perror("setsockopt error");
		return NITS_SOCKET_ERROR;
	}

	// Bind to the socket.
	if (bind(fd, res->ai_addr, res->ai_addrlen) < 0)
	{
		close(fd);
		perror("bind error");
		return NITS_SOCKET_ERROR;
	}

	// Listen on the socket.
	if (listen(fd, 5) < 0)
	{
		perror("listen error\n\n");
		return NITS_SOCKET_ERROR;
	}

	// Obtain the host and port and print it out.
	getnameinfo(res->ai_addr, res->ai_addrlen, gethost, sizeof gethost, getserv, sizeof getserv, NI_NUMERICSERV);
	printf ("Publisher created on %s %s\n\n", gethost, getserv);

	freeaddrinfo(res);
	listen_fd = fd;
	return NITS_SOCKET_OK;
}

/*
* Function that obtains the next subscriber and
* returns it's socket
*/
int get_next_subscriber (void)
{
	int accept_fd;

	printf ("Calling get_next_subscriber\n\n");

	// Return the error if the socket has not been created
	if (!(listen_fd > 0))
	{
		printf("The publisher has not been set up!\n");
		return NITS_SOCKET_ERROR;
	}

	// Accept the subscriber on obtain a socket.
	accept_fd = accept(listen_fd, NULL, NULL);

	// Return the error if the socket has not been created
	if (accept_fd < 0)
	{
		perror("Error accepting");
		return NITS_SOCKET_ERROR;
	}

	// Return the file descriptor.
	return accept_fd;
}

/*
* Function that sets up the discovery service.
*/
int setup_discovery (char *host, char *port)
{
	// Variables
	struct addrinfo hints, *res;
	int fd, status, val =1;
	char gethost[BUFFER_SIZE];
	char getserv[BUFFER_SIZE];

	printf ("Calling setup_discovery %s %s\n", host, port);

  // Zero out the hints and set the hints.
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	// Obtain the list of addresses.
	if ((status = getaddrinfo(host, port, &hints, &res)) != 0)
	{
		perror("getaddrinfo error");
		printf("status: %d", status);
		printf("%s\n", gai_strerror(status));
		return NITS_SOCKET_ERROR;
	}

	// Create a socket with the information from getaddrinfo.
	fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (fd == -1)
	{
		perror("socket error\n");
		return NITS_SOCKET_ERROR;
	}

  // Enable SO_REUSEADDR.
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
	{
		perror("setsockopt error");
		return NITS_SOCKET_ERROR;
	}

	// Bind to the socket.
	if (bind(fd, res->ai_addr, res->ai_addrlen) < 0)
	{
		close(fd);
		perror("bind error");
		return NITS_SOCKET_ERROR;
	}

	// Obtain the host and port and print it out.
	getnameinfo(res->ai_addr, res->ai_addrlen, gethost, sizeof gethost, getserv, sizeof getserv, NI_NUMERICSERV);
	printf ("Discovery service created on %s %s\n\n", gethost, getserv);
	return fd;
}

/*
 * Function that takes in the publishers host and port and creates
 * an ADVERTISE message and sends it to the discover service on
 * the specified host and port.
 */
int register_publisher (char *host, char *port, char *dhost, char *dport)
{
	// Variables
	int i, result, bytes, status;
	disc_advertise mesg;
	char pub_addr[ADDRESS_LENGTH];
	struct addrinfo hints, *res;
	struct sockaddr addr_cpy;
	int fd = -1, val = 1;
	char gethost[BUFFER_SIZE];
	char getserv[BUFFER_SIZE];

	printf ("Calling register_publisher %s %s %s %s\n", host, port, dhost, dport);

	// Zero out the hints and set the hints.
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	// Obtain the list of addresses.
	if ((status = getaddrinfo(dhost, dport, &hints, &res)) != 0)
	{
		perror("getaddrinfo error");
		printf("%s\n", gai_strerror(status));
		return NITS_SOCKET_ERROR;
	}

	// If the address is AF_LOCAL/UNIX then call setup discovery.
	// This will create the end point that is needed in order
	// to communicate properly. Otherwise just create a socket.
	if (res->ai_family == AF_UNIX)
		fd = setup_discovery(dhost, NULL);
	else
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (fd == NITS_SOCKET_ERROR)
	{
		perror("Error creating discovery service..exiting..");
		return NITS_SOCKET_ERROR;
	}

	// Establish a connection.
	if (connect(fd, res->ai_addr, res->ai_addrlen) < 0)
	{
		perror("connect error");
		return NITS_SOCKET_ERROR;
	}

	// Create the advertise message.
	strcpy(pub_addr, host);
	strcat(pub_addr, ":");
	strcat(pub_addr, port);
	mesg.msg_type = 'A';

	// Copy the address to the buffer.
	memcpy(&mesg.pub_address, &pub_addr, ADDRESS_LENGTH);

	// Send the message.
	bytes = write(fd, &mesg, sizeof(mesg));

	if (bytes < 0) {
		perror("send error");
		return NITS_SOCKET_ERROR;
	}

  // Obtain the host and port and print it out.
	getnameinfo(res->ai_addr, res->ai_addrlen, gethost, sizeof gethost, getserv, sizeof getserv, NI_NUMERICSERV);
	printf ("Sent the following: %s %s\nTo the discovery service:%s %s\n\n", host, port, gethost, getserv);

	freeaddrinfo(res);
	return (NITS_SOCKET_OK);
}

/*
 * Translates server:port to separate server and host pointers.
 * Note *host and *port must be writable strings (not constants)!
 */
int get_host_and_port (char *hostport, char **host, char **port)
{
	if (*hostport == ':')
	{
		*host = NULL;
		*port = &hostport[1];
		return(0);
	}
	*host = strtok(hostport, ":");
	*port = strtok(NULL, ":");
	return (0);
}
