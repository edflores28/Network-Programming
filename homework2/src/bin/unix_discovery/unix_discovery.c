/*
 *Project: Assignment 2
 *
 *Progam: unix_discovery
 *File Name: unix_discovery.c
 *Purpose: Service that communicates with both the
 *         publisher and subscriber via datagrams.
 *
 *Programmer: Edwin Flores
 *Course: EN.605.474.81
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "unixnitslib.h"

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
		int fd, recvlen;
  	char buffer[BUFFER_SIZE];
  	struct sockaddr_un client;
  	socklen_t size;
  	int nbytes;
  	disc_get_pub_list sub_mesg;
  	disc_advertise pub_mesg;
  	disc_pub_list disc_mesg;
  	struct sockaddr_un pub_addrs[NUM_PUBLISHERS];
  	int total_pubs = 0;

		client.sun_family = AF_LOCAL;
		size = sizeof(client);

  	fd = setup_discovery_server(DISCOVERY_PATH);

  	if (fd == NITS_SOCKET_ERROR)
  	{
    		perror("Error creating discovery service..exiting..");
    		exit(1);
  	}

  	// Wait for messages.
  	while(1)
  	{
			printf("Waiting for messages!\n");

    		// Read available data
    		nbytes = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client, &size);

    		// Process messages send from the publisher
    	if (nbytes == sizeof(pub_mesg))
    	{
				printf("Message received from the publisher.\n");

      	// Copy the buffer into the pub_mesg variable
     		memcpy(&pub_mesg, &buffer, sizeof(pub_mesg));

      	// If the publisher advertises it's address
      	// store it into the list and increment the counter
      	if ((pub_mesg.msg_type == ADVERTISE) && total_pubs < NUM_PUBLISHERS)
     		{
					printf("Received ADVERTISE\n");

        	pub_addrs[total_pubs] = pub_mesg.pub_address;
      		total_pubs++;
        	memset(&pub_mesg, 0, sizeof(pub_mesg));
      	}
    	}

   		// Process messages sent from the subscriber.
    	if (nbytes == sizeof(sub_mesg))
    	{
				printf("Message received from the subsciber.\n");

        // Copy the buffer into the sub_mesg variables
        memcpy(&sub_mesg, &buffer, sizeof(sub_mesg));

        // Send the publisher addresses to the subscriber
        // if it was requested.
        if (pub_mesg.msg_type == GET_PUB_LIST)
        {
					printf("Received GET_PUB_LIST\n");

          disc_mesg.msg_type = PUB_LIST;
          disc_mesg.num_publishers = total_pubs;

	  			memcpy(&disc_mesg.address[0], &pub_addrs, sizeof(disc_mesg.address));

					printf("Sending PUB_LIST\n");
          nbytes = sendto(fd, &disc_mesg, sizeof(disc_mesg), 0, (struct sockaddr*)&client, sizeof(client));

					if (nbytes < 0)
          	perror("error");

					// Clear the message
         	memset(&sub_mesg, 0, sizeof(sub_mesg));
      }
    }

    // Clear the buffer
    memset(&buffer, 0, sizeof(buffer));
 }
}
