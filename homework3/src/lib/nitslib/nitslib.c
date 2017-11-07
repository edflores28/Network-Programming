/*
 * Pretty header file stuff is here.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include "nitslib.h"

int setup_subscriber(char *host, char *port)
{
	printf ("Calling setup_subscriber %s %s\n", host, port);
	return (NITS_SOCKET_OK);
}

int setup_publisher(char *host, char *port)
{
	// Variables
	struct addrinfo hints, *res, *ptr;
	int status;

	memset(&hints, 0, sizeof (hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(host, port, &hints, &res);
	printf("status: %i %s\n", status, strerror(status));

	//if (status < 0)
	//{
	//	perror("getaddrinfo error\n");
	//	return NITS_SOCKET_ERROR;
	//}

	for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
	{
		printf("protocol %i\n", ptr->ai_family);
	}
	printf ("Calling setup_publisher %s %s\n", host, port);

	freeaddrinfo(res);
	return (NITS_SOCKET_OK);
}

int get_next_subscriber (void)
{
	printf ("Calling get_next_subscriber\n");
	return (NITS_SOCKET_OK);
}

int
setup_discovery (char *host, char *port)
{
	printf ("Calling setup_discovery %s %s\n", host, port);
	return (NITS_SOCKET_OK);
}

/*
 * Send message to discovery service
 * register host, port to discovery service on dhost, dport
 */
int
register_publisher (char *host, char *port, char *dhost, char *dport)
{
	printf ("Calling register_publisher %s %s %s %s\n", host, port,
					dhost, dport);
	return (NITS_SOCKET_OK);
}

#include <string.h>
/*
 * Translates server:port to separate server and host pointers.
 * Note *host and *port must be writable strings (not constants)!
 */
int
get_host_and_port (char *hostport, char **host, char **port)
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
