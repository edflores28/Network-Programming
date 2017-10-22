/*
*Project: Assignment 1
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
*Programmer: Edwin Flores
*Course: EN.605.474.81
*
*/
#define NITS_SOCKET_ERROR	(-1)
#define NITS_SOCKET_OK		(0)

int setup_subscriber(char *server_path);

int setup_publisher(char *server_path);

int get_next_subscriber(void);
