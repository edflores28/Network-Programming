/*
 *Project: Assignment 1
 *
 *Progam: unix_publisher
 *File Name: unix_publisher.c
 *Purpose: Serves articles requested by the publisher over
 *         a UNIX socket. The articles are looked in two
 *				 different directory paths.
 *
 *Programmer: Edwin Flores
 *Course: EN.605.474.81
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "unixnitslib.h"

#define ARRAY_SIZE 1024

void advertise() {
	struct sockaddr_un server, addr;

	// Copy the publisher path to the server_addr path and set
	strncpy(addr.sun_path, PATH, sizeof(addr.sun_path) - 1);

	disc_advertise mesg;

	mesg.msg_type = ADVERTISE;
  mesg.pubaddr_size = sizeof(addr.sun_path);
	mesg.pub_address = addr;
	
	int udp_fd = setup_discovery_server("/home/eflores4/discli");

	server.sun_family = AF_LOCAL;
	strncpy(server.sun_path, DISCOVERY_PATH, sizeof(server.sun_path) - 1);
	int nbytes;
	//	while(1){
	nbytes = sendto(udp_fd, &mesg, sizeof(mesg), 0, (struct sockaddr *)&server, sizeof(server));
	//	}
	printf("BYTES: %i\n",nbytes);
	exit(1);

}

int main(int argc, char *argv[])
{
	// Variables
	int fd;
	int bytes;
	char buffer[ARRAY_SIZE];
	char article[ARRAY_SIZE];
	FILE *file;
	int found = -1;

	// Path's to look for the articles
	char myArticle[] = "/home/eflores4/Articles/";
	char netArticle[] = "/home/net_class/474/Articles/";

	if (setup_publisher (PATH) == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "Error setting up the publisher.\n");
		exit(1);
	}

// Run the loop until terminated
	while(1)
	{
			// Obtain the file descriptor for the subscriber.
			// Exit is there is an error.
			fd = get_next_subscriber();

			if (fd == NITS_SOCKET_ERROR)
			{
				perror("Error obtaining socket");
				exit(1);
			}

			// Clear the buffer and read which article the subscriber requested
			memset(&buffer[0],0,sizeof(buffer));
			bytes = read(fd, buffer, ARRAY_SIZE-1);

			if (bytes < 0)
				printf("Error Reading\n");

			printf("Recieved %s from the subscriber\n", buffer);

			// Check to see if QUIT was received,
			// If so break from the while loop
			if ((bytes == 4) && (buffer[0] == 'Q') && (buffer[1] == 'U') && (buffer[2] == 'I') && (buffer[3] == 'T'))
			{
				printf("QUIT received, exiting..\n");
				break;
			}

			// Clear the article buffer and determine
			// if the article is found in "my" path, this
			// checks to see if the file exists and is readable.
			memset(&article[0], 0, sizeof(article));
			strcpy(article, myArticle);
			strncat(article, buffer, bytes);

			found = access(article,F_OK|R_OK);

			// Check to see if the article was found. If not
			// clear the article buffer and determine
			// if the article is found in "net_class" path.
			if (found == -1)
			{
				memset(&article[0], 0, sizeof(article));
				strcpy(article, netArticle);
				strncat(article, buffer, bytes);
				found = access(article,F_OK|R_OK);
			}

			// If the article is found send it to the subscriber.
			if (found == 0)
			{
				// Obtain a handle to the file.
				file = fopen(article,"rb");

				if (file == NULL)
				{
					perror("Error Opening File");
					exit(1);
				}

				// Transferring the requested article.
				while(fgets(buffer, sizeof(buffer),file))
					write(fd,buffer,ARRAY_SIZE-1);

				printf("Finished sending to the subscriber\n");
				close(fd);
			}

			// Close the file descriptor if the article is
			// not found in both paths.
			if (found != 0)
			{
				printf("File path: %s not found..\n",buffer);
				close(fd);
			}
	}

	// Do some cleanup.
	close(fd);
	fclose(file);
	exit (0);
}
