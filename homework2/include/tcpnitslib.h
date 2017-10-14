/*
*Project: Assignment 2
*
*Library: tcpnitslib
*File Name: tcpnitslib.h
*Purpose: provides utilities to set up TCP client
*         and server sockets
*
*Synopsis (Usage and Parameters):
*
*	  setup_subscriber(struct in_addr *host, int port)
*
* 	setup_publisher(int port)
*
* 	get_next_subscriber(void)
*
*   setup_discovery_server(int port)
*
*Programmer: Edwin Flores
*Course: EN.605.474.81
*
*/
#include <netinet/in.h>

#define NITS_SOCKET_ERROR	(-1)
#define NITS_SOCKET_OK		(0)
#define PUBLISHER_PORT		(8404)
#define DISCOVERY_PORT		(8404)

/*
 * Discovery service handles this many publishers
*/
#define NUM_PUBLISHERS 5


int setup_subscriber (struct in_addr *host, int port);

int setup_publisher (int port);

int get_next_subscriber(void);

int setup_discovery_server(int port);
