/*
 *	<Header stuff goes here>
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "tcpnitslib.h"

#define MY_PORT	7640

#define ARRAY_SIZE 256

int main(int argc, char *argv[])
{

	int fd;
	int bytes;
	char buffer[ARRAY_SIZE];
	char article[ARRAY_SIZE];
	FILE *file;
	int found = -1;

	char myArticle[] = "/home/eflores4/Articles/";
	char netArticle[] = "/home/net_class/474/Articles/";

	if ((fd = setup_publisher (MY_PORT)) == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "Error setting up the publisher.\n");
		exit(1);
	}

	while(1)
	{
			// Obtain the file descriptor. Exit is there is an error.
			fd = get_next_subscriber();

			if (fd == NITS_SOCKET_ERROR)
			{
				perror("Error obtaining socket");
				exit(1);
			}

			memset(&buffer[0],0,sizeof(buffer));
			// Read which article the subscriber requested.
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

	close(fd);
	fclose(file);
	exit (0);
}
