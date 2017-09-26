/*
 * Pretty header file stuff is here.
 */
#include <stdio.h>
#include "tcpnitslib.h"

int setup_subscriber(struct in_addr *host, int port)
{
	printf ("Setting up tcp subscriber on %d\n", port);
	return (0);
}

int setup_publisher(int port)
{
	printf ("Setting up tcp publisher on %d\n", port);
	return (0);
}
