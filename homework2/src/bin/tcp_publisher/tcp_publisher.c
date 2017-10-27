/*
 *Project: Assignment 1
 *
 *Progam: tcp_publisher
 *File Name: tcp_publisher.c
 *Purpose: Serves articles requested by the publisher over
 *         a TCP socket. The articles are looked in two
 *				 different directory paths.
 *
 *Programmer: Edwin Flores
 *Course: EN.605.474.81
 *
 */
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "tcpnitslib.h"
#include "config.h"
#include "utillib.h"

#define BUFFER_SIZE 1024

// Path's to look for the articles
char myArticle[] = "/home/eflores4/Articles/";
char netArticle[] = "/home/net_class/474/Articles/";

/**
* This method creates a datagram socket that will
* connect to the dicovery service. The publisher will
* send it's address to the publisher.
*/
void advertise(char * disc_addr) {

	// Variables
	struct sockaddr_in server, addr;
	char buffer[256];
	disc_advertise mesg;
	int nbytes, fd, res;
	int len = sizeof(struct sockaddr);
	struct hostent *host;
	char **ptr;
	char str[256];

	// Clear the buffer
	memset(&buffer[0],0, sizeof(buffer));

	// Get the hostname of the publisher
	res = gethostname(buffer, sizeof(buffer));

	if (res < 0)
	{
		perror("Error obtaining hosname\n");
		exit(1);
	}

	// Obtain hostent which contains all the info on
	// the publisher.
	host = gethostbyname(buffer);

	if (host == NULL)
	{
		perror("Error obtaining hostent");
		exit(1);
	}

	ptr = host->h_addr_list;
	for (; *ptr != NULL; ptr++)
		printf("addr %s\n",inet_ntop(host->h_addrtype, *ptr, str,sizeof(str)));


	// Fill in the addr struct, this data is sent to the
	// discovery service.
	memcpy(&addr.sin_addr, host->h_addr_list[0], host->h_length);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(TCP_PORT);


	// Fill in the message structure.
	mesg.msg_type = ADVERTISE;
 	mesg.pubaddr_size = sizeof(addr);
	mesg.pub_address = addr;

	// Create the client
	fd = setup_discovery_server(UDP_PORT);

	if (fd == NITS_SOCKET_ERROR)
	{
		perror("Error creating socket..\n");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(UDP_PORT);

	if (inet_aton(disc_addr,&server.sin_addr) == 0)
	{
		perror("inet_aton error..exit..\n");
		exit(1);
	}

	// Send the message to the discovery service.
	nbytes = sendto(fd, &mesg, sizeof(mesg), 0, (struct sockaddr *)&server, sizeof(server));

	if (nbytes < 0)
	{
		perror("Error advertising to the publisher..exiting.\n");
		exit(1);
	}

	close(fd);
}

/**
* This method is the method that the child process will execute.
* This handles the communication to the subscriber.
*/
void child_process(int fd)
{
	// Variables
	int bytes;
	char buffer[BUFFER_SIZE];
	char article[BUFFER_SIZE];
	FILE *file;
	int found = -1;

	while(1)
	{
		// Clear the buffer and read which article the subscriber requested
		memset(&buffer[0],0,sizeof(buffer));
		bytes = read(fd, buffer, BUFFER_SIZE);

		if (bytes < 0)
		{
			printf("Error Reading.. exiting..\n");
			exit(1);
		}

		printf("Recieved %s from the subscriber\n", buffer);

		// Check to see if QUIT was received, If so
		// close the socket and send the term signal
		// to the parent process.
		if (strcmp("QUIT", buffer) == 0)
		{
			printf("QUIT received, exiting..\n");
			close(fd);
			kill(getppid(), SIGKILL);
			exit(1);
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
			while(1)
			{
				bytes = fread(buffer, sizeof(char), BUFFER_SIZE, file);

				// Break from the loop is no bytes are read.
				if (bytes == 0)
					break;

				// Send to the subscriber
				bytes = write(fd,buffer,BUFFER_SIZE);

				// Clear the buffer.
				memset(&buffer, 0, sizeof(buffer));
			}

			printf("Finished sending to the subscriber\n");
			fclose(file);
		}

		// Print a message if the article was not found.
		if (found != 0)
		{
			printf("File path: %s not found..\n",buffer);
		}
	}
}

/**
* Main Program.
*/
int main(int argc, char *argv[])
{
	// Variables
	int fd;
	pid_t pID;
	char *disc_addr;

	disc_addr = parse_arg(argc, argv);

	if (disc_addr == NULL) {
		printf("I AM NULL")
		disc_addr = DEFAULT_TCP_DISC;
	}

	advertise(disc_addr);

	// Obtain the socket, exit if there is an error.
	if ((fd = setup_publisher (TCP_PORT)) == NITS_SOCKET_ERROR)
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

			// Create a child process
			pID = fork();

			// Exit if there is an error.
			if (pID < 0){
				perror("Failed to fork!\n");
				exit(1);
			}

			// Have the child process do it's work.
			if (pID == 0){
				child_process(fd);
			}

			// Parent has no need for the file desciptor
			close(fd);
	}
	exit (0);
}
