#include "discovery.h"
#include "unixnitslib.h"

#define DEFAULT_DISCOVERY_ADDR 128.220.101.247
#define BUFFER_SIZE 256

int main(int argc, char *argv[])
{
  int fd, recv_len;
  char buffer[BUFFER_SIZE];

  fd = setup_discovery_server(DISCOVERY_PATH);

  if (fd == NITS_SOCKET_ERROR)
  {
    printf("Error creating discovery service..exiting..");
    exit(1);
  }

  while(1)
  {
      printf("Waiting to receive a message");
      recvlen = recvfrom(fd, buffer, BUFFER_SIZE, 0, NULL, NULL);

      for (int i = 0; i < recvlen; i++){
        printf(buffer[i]);
      }
  }
}
