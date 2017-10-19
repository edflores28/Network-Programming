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

typedef enum {GET_PUB_LIST, PUB_LIST, ADVERTISE} DISCOVERY_MESSAGES;

/*
 * Message sent from subscriber to discovery service
 */

typedef struct {
	DISCOVERY_MESSAGES  msg_type;	/* GET_PUB_LIST */
} disc_get_pub_list;

/*
 * Message sent from discovery service to subscriber
 */
typedef struct {
	DISCOVERY_MESSAGES  msg_type;	/* PUB_LIST */
	int num_publishers;
	struct sockaddr_in	address[NUM_PUBLISHERS];
} disc_pub_list;

/*
 * Message sent from publisher to discovery service
 */
typedef struct {
	DISCOVERY_MESSAGES  msg_type;		/* ADVERTISE */
	int pubaddr_size;			/* size of address */
	struct sockaddr_in	pub_address;	/* Address of publisher */
} disc_advertise;

typedef union {
	disc_get_pub_list getlist;
	disc_pub_list publist;
	disc_advertise putpub;
} discovery_msgs;

int setup_subscriber (struct in_addr *host, int port);

int setup_publisher (int port);

int get_next_subscriber(void);

int setup_discovery_server(int port);
