/*
 *Project: Assignment 4
 *
 *Progam: mcast_cli
 *File Name: mcast_cli.c
 *Purpose: Group of MAX_CLIENTS that reads and prints
 *         the multicast message.
 *
 *Programmer: Edwin Flores
 *Course: EN.605.474.81
 *
 */
#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "config.h"

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 15

// Struct that contains the information of the client.
struct info {
  int fd;
  struct sockaddr_in client;
  int addrlen;
};

// Array of structs for each client.
struct info list[MAX_CLIENTS];

/**
  The method creates a socket, binds the socket, and joins
  a the multicast group for MAX_CLIENTS.
*/
int setup_clients()
{
  int i, result, val = 1;
  struct sockaddr_in c_addr;
  struct ip_mreq multi;
  int total_setup = 0;
  int port = 8000;

  // Zero out.
  memset(&c_addr, 0, sizeof(c_addr));

  // Set the family and the address.
  c_addr.sin_family = AF_INET;
  c_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  // Define the multicast group.
  multi.imr_multiaddr.s_addr=inet_addr(MULTICAST_GROUP);
  multi.imr_interface.s_addr=htonl(INADDR_ANY);

  // Create each client.
  for (i = 0; i < MAX_CLIENTS; i++)
  {
    // Clear out client so it can be used.
    memset(&list[i].client, 0, sizeof(list[i].client));

    // Set the port for the socket.
    c_addr.sin_port = htons(port);

    // Set the options.
    list[i].client = c_addr;
    list[i].addrlen = sizeof(list[i].client);

    // Increment the port;
    port++;

    // Obtain a file descriptor for the client. goto the end of
    // the loop is an error is encountered.
    if ((list[i].fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
      perror("Error creating socket");
      goto end;
    }

    // Set the SO_REUSEADDR option. goto the end of
    // the loop is an error is encountered.
    if (setsockopt(list[i].fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
    {
      perror("setsockopt error");
      goto end;
    }

    // Bind to the socket. goto the end of
    // the loop is an error is encountered.
    if (result = bind(list[i].fd, (struct sockaddr*)&list[i].client, list[i].addrlen) == -1)
    {
      perror("Error binding");
    }

    // Join the multicast group. goto the end of
    // the loop is an error is encountered.
    if (setsockopt(list[i].fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&multi,sizeof(multi)) < 0) {
      perror("Setsockopt multicast add error");
      goto end;
    }

    // Increment the counter.
    total_setup++;

    printf("Created a client on port %d\n", port);
    printf("Joined multicast group %s\n\n", MULTICAST_GROUP);

    end : NULL;
  }

  return total_setup;
}

/**
  The main program.
*/
int main (int argc, char *argv[])
{
  // Variables.
  int fd, total_cli, val = 1;
  int i, c, result, max_fd = 0;
  int nbytes, addrlen;
  char buffer[BUFFER_SIZE];
  fd_set readfds;

  // Setup all the clients.
  total_cli = setup_clients();

  // Exit if there are no available clients.
  if (total_cli == 0)
  {
    printf("There are no clients setup!\n");
    return 1;
  }

  while(1)
  {
    // Clear out the descriptor list.
    FD_ZERO(&readfds);

    // Check each fd if it's the largets value
    // and it to the set.
    for (i = 0; i < MAX_CLIENTS; i++)
    {
      if (list[i].fd > max_fd)
        max_fd = list[i].fd;

      if (list[i].fd > 0)
        FD_SET(list[i].fd, &readfds);
    }

    // Add STDIN file descriptor to be monitored.
    FD_SET(STDIN_FILENO, &readfds);

    // Call select
    result = select(max_fd+1, &readfds, NULL, NULL, NULL);

    // Exit if it's the STDIN fd.
    if (FD_ISSET(STDIN_FILENO, &readfds))
    {
      printf("Activity on STDIN, exiting..\n");

      for (i = 0; i < MAX_CLIENTS; i++)
        if (list[i].fd > 0)
          close(list[i].fd);

      return 0;
    }

    // For each available fd determine if it's set and recv the message
    // and print it out.
    for (i = 0;i < MAX_CLIENTS; i++)
    {
      if (FD_ISSET(list[i].fd, &readfds))
      {
        memset(&buffer, 0, BUFFER_SIZE);

        if((nbytes = recvfrom(list[i].fd, buffer, BUFFER_SIZE,0,(struct sockaddr*)&list[i].client, &list[i].addrlen)) < 0)
        {
          perror("recvfrom error");
          close(fd);
        }
        else
          printf("Activity on fd %d -- Server sent time: %s\n", list[i].fd, buffer);
      }
    }
  }
}
