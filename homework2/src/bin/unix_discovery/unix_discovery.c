#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "discovery.h"
#include "unixnitslib.h"
#include <errno.h>

#define DEFAULT_DISCOVERY_ADDR 128.220.101.247
#define BUFFER_SIZE 256

int main(int argc, char *argv[])
{
  int fd, recvlen;
  char buffer[BUFFER_SIZE];
  int error;
  
  fd = setup_discovery_server(DISCOVERY_PATH);

  error = errno;
  int i;
  if (fd == NITS_SOCKET_ERROR)
  {
    printf("Error creating discovery service..exiting..%i",error);
    exit(1);
  }

  while(1)
  {
      printf("Waiting to receive a message");
      recvlen = recvfrom(fd, buffer, BUFFER_SIZE, 0, NULL, NULL);

      for (i = 0; i < recvlen; i++){
        printf("%i",buffer[i]);
      }
  }
}
