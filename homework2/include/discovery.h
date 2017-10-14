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
