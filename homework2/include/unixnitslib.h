/*
*Project: Assignment 2
*
*Library: unixnitslib
*File Name: unixnitslib.h
*Purpose: provides utilities to set up UNIX client
*         and server sockets
*
*Synopsis (Usage and Parameters):
*
*	  setup_subscriber(char *publisher_path)
*
* 	setup_publisher(char *publisher_path)
*
* 	get_next_subscriber(void)
*
*   setup_discovery_server(char *server_socket)
*
*Programmer: Edwin Flores
*Course: EN.605.474.81
*
*/
#define NITS_SOCKET_ERROR	(-1)
#define NITS_SOCKET_OK		(0)

#define PATH "/home/eflores4/serversocket"

#define DISCOVERY_PATH	"/home/eflores4/disc"

int setup_subscriber(char *server_path);

int setup_publisher(char *server_path);

int get_next_subscriber(void);

int setup_discovery_server(char *server_socket);
