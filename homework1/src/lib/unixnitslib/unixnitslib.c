//#include <sys/socket.h>
#include <stdio.h>
#include "unp.h"
#include "unixnitslib.h"

int setup_subscriber(char *publisher_path)
{
	printf ("Setting up unix domain subscriber on %s\n", publisher_path);
	return (0);
}

int setup_publisher(char *publisher_path)
{
	int server;
	struct sockaddr_un server_addr;

	server = socket(AF_LOCAL, SOCKS_STREAM, 0);
	printf ("Setting up unix domain publisher on %s\n", publisher_path);
	return (0);
}
