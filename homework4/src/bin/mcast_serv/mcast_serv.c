/*
 *Project: Assignment 4
 *
 *Progam: mcast_serv
 *File Name: mcast_serv.c
 *Purpose: Server that sends it's localtime every 5 seconds
 *         to a multicast address.
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
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "config.h"

/**
  The main program.
*/
int main (int argc, char *argv[])
{
  // Variables
  int c, nbytes, i;
  int fd, result;
  struct sockaddr_in server;
  int val = 1;
  char *port = NULL;
  time_t rawtime;
  struct tm *info;
  char *date;
  int length;

  // Check and parse if there are and command line options.
	while ((c = getopt(argc, argv, "p:")) != -1)
	{
		switch (c)
		{
				case 'p':
				      port = optarg;
				      break;
				default:
				      fprintf (stderr, "Usage: %s -p <port>\n", argv[0]);
				      exit (1);
		}
	}

  if (port == NULL)
    port = DEFAULT_PORT;

  // Obtain a file descriptor for the server.
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Error creating socket");
		exit(1);
	}

  printf("Socket created!\n");
  printf("Sending messages to multicast address %s port %s\n", MULTICAST_GROUP, port);

	// Clear out server so it can be used.
	memset(&server, 0, sizeof(server));

	// Set the family and the port number.
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(port));
  server.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

  while(1)
  {
    // Obtain the server's time information
    time (&rawtime);
    info = localtime(&rawtime);
    date = asctime(info);
    length = 0;

    // Determine the length of the date.
    for (i = 0; i < 128; i++)
    {
      if (date[i] == '\0')
        break;
      length++;
    }

    // Send the date.
    nbytes = sendto(fd, date, length, 0, (struct sockaddr *)&server, sizeof(server));

    if (nbytes < 0)
    {
      perror("Sendto error");
      return 1;
    }

    // Sleep for 5 seconds.
    sleep(5);
  }
}
