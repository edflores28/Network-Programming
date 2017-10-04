/*
 * Yeah, there should be pretty header comments, here, too.
 */
#define NITS_SOCKET_ERROR	(-1)
#define NITS_SOCKET_OK		(0)

#define PATH "/home/eflores4/serversocket"

int setup_subscriber(char *server_path);

int setup_publisher(char *server_path);

int get_next_subscriber(void);
