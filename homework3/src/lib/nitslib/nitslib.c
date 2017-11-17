/*
 * Pretty header file stuff is here.
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

// Global Variables
static int listen_fd;

int setup_subscriber(char *host, char *port)
{
	int i, result, fd, status, val = 1;
	struct addrinfo hints, *res, *ptr;
	struct sockaddr addr_cpy;
	enum BOOL found = FALSE;

	memset(&hints, 0, sizeof (hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(host, port, &hints, &res)) != 0)
	{
		perror("getaddrinfo error");
		printf("%s\n", gai_strerror(status));
		return NITS_SOCKET_ERROR;
	}

	for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
	{
		if ((ptr->ai_family == AF_INET) || ptr->ai_family == AF_INET6
	       || ptr->ai_family == AF_UNIX)
		{
			found = TRUE;
			fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			break;
		}
	}

	if (found == FALSE)
	{
		printf("Unable to find anything in getaddrinfo\n");
		return NITS_SOCKET_ERROR;
	}

	if (fd == -1)
	{
		perror("socket error");
		return NITS_SOCKET_ERROR;
	}

	if (fd == NITS_SOCKET_ERROR)
		return NITS_SOCKET_ERROR;

	for (i = 0; i < 6; i++)
	{
		result = connect(fd, ptr->ai_addr, ptr->ai_addrlen);
		printf("result: %d\n", result);
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

	printf ("Calling setup_subscriber %s %s\n", host, port);
	freeaddrinfo(res);
	return fd;
}

int setup_publisher(char *host, char *port)
{
	// Variables
	struct addrinfo hints, *res, *ptr;
	int fd, status, val =1;
	enum BOOL found = FALSE;

	memset(&hints, 0, sizeof (hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	printf("%s, %s ", host, port);
	if ((status = getaddrinfo(host, port, &hints, &res)) != 0)
	{
		perror("getaddrinfo error");
		printf("status: %d", status);
		printf("%s\n", gai_strerror(status));
		return NITS_SOCKET_ERROR;
	}

	for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
	{
		if ((ptr->ai_family == AF_INET) || ptr->ai_family == AF_INET6
	       || ptr->ai_family == AF_UNIX)
		{
			found = TRUE;
			fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (fd == -1)
			{
				perror("socket error\n");
				return NITS_SOCKET_ERROR;
			}

			if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0){
				perror("setsockopt error");
				return NITS_SOCKET_ERROR;
			}

			if (bind(fd, ptr->ai_addr, ptr->ai_addrlen) < 0)
			{
				close(fd);
				perror("bind error");
				return NITS_SOCKET_ERROR;
			}
			break;
		}
	}

	if (found == FALSE)
	{
		printf("Unable to find anything in getaddrinfo\n");
		return NITS_SOCKET_ERROR;
	}

	if (listen(fd, 5) < 0)
	{
		perror("listen error\n\n");
		return NITS_SOCKET_ERROR;
	}
	printf ("Calling setup_publisher %s %s\n", host, port);

	freeaddrinfo(res);
	listen_fd = fd;
	return NITS_SOCKET_OK;
}

int get_next_subscriber (void)
{
	int accept_fd;

	printf ("Calling get_next_subscriber\n");
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

	return accept_fd;
}

int setup_discovery (char *host, char *port)
{
	// Variables
	struct addrinfo hints, *res, *ptr;
	int fd, status, val =1;
	enum BOOL found = FALSE;

	memset(&hints, 0, sizeof (hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	printf("%s, %s ", host, port);
	if ((status = getaddrinfo(host, port, &hints, &res)) != 0)
	{
		perror("getaddrinfo error");
		printf("status: %d", status);
		printf("%s\n", gai_strerror(status));
		return NITS_SOCKET_ERROR;
	}

	for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
	{
		if ((ptr->ai_family == AF_INET) || ptr->ai_family == AF_INET6
	       || ptr->ai_family == AF_UNIX)
		{
			found = TRUE;

			fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (fd == -1)
			{
				perror("socket error\n");
				return NITS_SOCKET_ERROR;
			}

			if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0){
				perror("setsockopt error");
				return NITS_SOCKET_ERROR;
			}

			if (bind(fd, ptr->ai_addr, ptr->ai_addrlen) < 0)
			{
				close(fd);
				perror("bind error");
				return NITS_SOCKET_ERROR;
			}
			break;
		}
	}

	if (found == FALSE)
	{
		printf("Unable to find anything in getaddrinfo\n");
		return NITS_SOCKET_ERROR;
	}

	printf ("Calling setup_discovery %s %s\n", host, port);
	return fd;
}

/*
 * Send message to discovery service
 * register host, port to discovery service on dhost, dport
 */
int register_publisher (char *host, char *port, char *dhost, char *dport)
{
	int i, result, bytes, status;
	disc_advertise mesg;
	char pub_addr[ADDRESS_LENGTH];
	struct addrinfo hints, *res, *ptr;
	struct sockaddr addr_cpy;
	int fd = -1, val = 1;
	enum BOOL found = FALSE;

	memset(&hints, 0, sizeof (hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((status = getaddrinfo(host, port, &hints, &res)) != 0)
	{
		perror("getaddrinfo error");
		printf("%s\n", gai_strerror(status));
		return NITS_SOCKET_ERROR;
	}

	for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
	{
		if ((ptr->ai_family == AF_INET) || ptr->ai_family == AF_INET6
		     || ptr->ai_family == AF_UNIX)
		{
			found = TRUE;

			fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (fd == -1)
			{
				perror("socket error");
				return NITS_SOCKET_ERROR;
			}

			if (ptr->ai_family == AF_UNIX)
				unlink(dport);

			if (bind(fd, ptr->ai_addr, ptr->ai_addrlen) < 0)
			{
				close(fd);
				perror("bind error");
				return NITS_SOCKET_ERROR;
			}

			break;
		}
	}

	if (found == FALSE)
	{
		printf("Unable to find anything in getaddrinfo\n");
		return NITS_SOCKET_ERROR;
	}

	// Create the advertise message.
	strcpy(pub_addr, host);
	strcat(pub_addr, ":");
	strcat(pub_addr, port);

	mesg.msg_type = 'A';
	memcpy(&mesg.pub_address, &pub_addr, ADDRESS_LENGTH);

	bytes = sendto(fd, &mesg, sizeof(mesg), 0, ptr->ai_addr, ptr->ai_addrlen);

	if (bytes < 0) {
		perror("send error");
		return NITS_SOCKET_ERROR;
	}

	printf ("Calling register_publisher %s %s %s %s\n", host, port,
					dhost, dport);
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
