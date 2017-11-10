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

// Global Variables
static int listen_fd;

int get_family(char *host)
{
	if (strcmp("/unix",host) == 0)
		return AF_UNIX;

	if (strcmp("/local",host) == 0)
		return AF_LOCAL;

	return AF_INET;
}

int server_setup(char *host, char *port, int sock_type)
{
	struct addrinfo hints, *res, *ptr;
	int fd, status, val =1;

	memset(&hints, 0, sizeof (hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = sock_type;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(host, port, &hints, &res)) != 0)
	{
		perror("getaddrinfo error\n");
		printf("%s\n", gai_strerror(status));
		return NITS_SOCKET_ERROR;
	}

	for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
	{
		printf("protocol: %i ai_family: %i name: %s\n", ptr-> ai_protocol, ptr->ai_family, ptr->ai_canonname);
		if (ptr->ai_family == get_family(host))
		{
			fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			printf("%d\n", fd);
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
	freeaddrinfo(res);
	return fd;
}

int setup_subscriber(char *host, char *port)
{
	// Variables
	struct addrinfo hints, *res, *ptr;
	int status, result, sub_fd, i;

	memset(&hints, 0, sizeof (hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(host, port, &hints, &res)) != 0)
	{
		perror("getaddrinfo error\n");
		printf("%s\n", gai_strerror(status));
		return NITS_SOCKET_ERROR;
	}

	for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
	{
		printf("protocol: %i ai_family: %i name: %s\n", ptr-> ai_protocol, ptr->ai_family, ptr->ai_canonname);
		if (ptr->ai_family == get_family(host))
		{
			sub_fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (sub_fd == -1)
			{
				perror("socket error\n");
				return NITS_SOCKET_ERROR;
			}

			for (i = 0; i < 6; i++)
			{
				result = connect(sub_fd, ptr->ai_addr, ptr->ai_addrlen);
				if (result == 0)
					break;
				else
				{
					perror("connect error.");
					sleep(1);
				}
			}
			break;
		}
	}

		// If the counter is set to 6 return an error.
		if (i == 6)
			return NITS_SOCKET_ERROR;

		printf ("Calling setup_subscriber %s %s\n", host, port);
		return sub_fd;
	//}
}

int setup_publisher(char *host, char *port)
{
	// Variables
	int fd;

	if ((fd = server_setup(host, port, SOCK_STREAM)) == NITS_SOCKET_ERROR)
		return NITS_SOCKET_ERROR;

	if (listen(fd, 5) < 0)
	{
		perror("listen error\n\n");
		return NITS_SOCKET_ERROR;
	}
	printf ("Calling setup_publisher %s %s\n", host, port);

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
	int fd;

	if ((fd = server_setup(host, port, SOCK_DGRAM)) == NITS_SOCKET_ERROR)
		return NITS_SOCKET_ERROR;

	printf ("Calling setup_discovery %s %s\n", host, port);
	return fd;
}

/*
 * Send message to discovery service
 * register host, port to discovery service on dhost, dport
 */
int register_publisher (char *host, char *port, char *dhost, char *dport)
{
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
