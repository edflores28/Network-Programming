/*
 * Pretty header stuff goes here.
 */
#include <netinet/in.h>
#define NITS_SOCKET_ERROR	(-1)
#define NITS_SOCKET_OK		(0)

int setup_subscriber (struct in_addr *host, int port);
int setup_publisher (int port);
int get_next_subscriber(void);
