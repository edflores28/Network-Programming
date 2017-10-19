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

#define DISCOVERY_PATH	"/tmp/port.8404_d"
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
	struct sockaddr_un	address[NUM_PUBLISHERS];
} disc_pub_list;

/*
 * Message sent from publisher to discovery service
 */
typedef struct {
	DISCOVERY_MESSAGES  msg_type;		/* ADVERTISE */
	int pubaddr_size;			/* size of address */
	struct sockaddr_un	pub_address;	/* Address of publisher */
} disc_advertise;

typedef union {
	disc_get_pub_list getlist;
	disc_pub_list publist;
	disc_advertise putpub;
} discovery_msgs;

int setup_subscriber(char *server_path);

int setup_publisher(char *server_path);

int get_next_subscriber(void);

int setup_discovery_server(char *server_socket);
