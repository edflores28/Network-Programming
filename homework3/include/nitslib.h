/*
 * Nitslib.h
 * Protocol independent Network Information Transfer System
 */
#define NITS_SOCKET_ERROR	(-1)
#define NITS_SOCKET_OK		(0)
/*
 * Discovery service handles this many publishers
*/
#define NUM_PUBLISHERS 5

/* typedef enum {GET_PUB_LIST, PUB_LIST, ADVERTISE} DISCOVERY_MESSAGES; */
typedef char DISCOVERY_MESSAGES;
#define GET_PUB_LIST 'G'
#define PUB_LIST 'P'
#define ADVERTISE 'A'

/*
 * Message sent from subscriber to discovery service
 */

typedef struct {
	DISCOVERY_MESSAGES  msg_type;	/* GET_PUB_LIST */
} disc_get_pub_list;

/*
 * Message sent from discovery service to subscriber
 * Address of publishers is an ascii string.
 */
#define ADDRESS_LENGTH	64
typedef char ADDRESS[ADDRESS_LENGTH];
/*
 * Address is an ascii string of the form:
 * host:port
 * E.g. for IP absaroka.apl.jhu.edu:4690, for UNIX /unix:/tmp/port7460
 */
typedef struct {
	DISCOVERY_MESSAGES  msg_type;	/* PUB_LIST */
	char num_publishers[3];		/* number of publishers -an ascii string*/
	ADDRESS publisher_address[NUM_PUBLISHERS]; /* an array of characters */
} disc_pub_list;

/*
 * Message sent from publisher to discovery service
 */
typedef struct {
	DISCOVERY_MESSAGES  msg_type;		/* ADVERTISE */
	ADDRESS	pub_address;			/* Address of publisher */
} disc_advertise;

typedef union {
	disc_get_pub_list getlist;
	disc_pub_list publist;
	disc_advertise putpub;
} discovery_msgs;

int setup_subscriber (char *host, char *port);
int setup_publisher (char *host, char *port);
int setup_discovery (char *host, char *port);
int register_publisher (char *host, char *port, char *dhost, char *dport);
int get_host_and_port (char *hostport, char **host, char **port);
