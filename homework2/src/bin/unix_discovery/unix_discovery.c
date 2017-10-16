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
  
  fd = setup_discovery_server(DISCOVERY_PATH);

  printf("HELLO %i\n", fd);
 
  if (fd == NITS_SOCKET_ERROR)
  {
    perror("Error creating discovery service..exiting..");
    exit(1);
  }

  while(1){
  int nbytes = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client, &size);

  printf("bytes: %i\n",nbytes);
 }
}
