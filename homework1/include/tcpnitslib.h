/*
*Project: Assignment 1
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
*Programmer: Edwin Flores
*Course: EN.605.474.81
*
*/
#include <netinet/in.h>
#define NITS_SOCKET_ERROR	(-1)
#define NITS_SOCKET_OK		(0)

int setup_subscriber (struct in_addr *host, int port);
int setup_publisher (int port);
int get_next_subscriber(void);
