#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "discovery.h"
#include "unixnitslib.h"
#include <errno.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[])
{
  int fd, recvlen;
  char buffer[BUFFER_SIZE];
  struct sockaddr_un client;
  socklen_t size;
  int nbytes;
  disc_get_pub_list sub_mesg;
  disc_advertise pub_mesg;

  struct sockaddr_un pub_addresses[NUM_PUBLISHERS];
  int total_pubs = 0;

  fd = setup_discovery_server(DISCOVERY_PATH);

  if (fd == NITS_SOCKET_ERROR)
  {
    perror("Error creating discovery service..exiting..");
    exit(1);
  }

  // Wait for messages.
  while(1){
    nbytes = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client, &size);

    if (nbytes == sizeof(pub_mesg)){
      
      memcpy(&pub_mesg, &buffer, sizeof(pub_mesg));
      
      if (pub_mesg.msg_type == ADVERTISE)
      {
         
      }
        
    }
    
    // Clear the buffer
    memset(&buffer, 0, sizeof(buffer));
  }
}
