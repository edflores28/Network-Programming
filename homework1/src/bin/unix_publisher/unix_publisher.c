/*
 *	<Header stuff goes here>
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "unixnitslib.h"

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

	if (setup_publisher (PATH) == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "Stupid idiot, you screwed up.\n");
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

			// Read which article the subscriber requested.
			bytes = read(fd, buffer, ARRAY_SIZE-1);

			if (bytes < 0)
				printf("Error Reading");

			printf("Recieved %s from the subscriber\n", buffer);
			// Check to see if QUIT was received,
			// If so break from the while loop
			if ((bytes == 4) && (buffer[0] == 'Q') && (buffer[1] == 'U') && (buffer[2] == 'I') && (buffer[3] == 'T'))
			{
				printf("QUIT received, exiting..");
				break;
//				exit(0);
			}

			// Clear the article buffer and determine
			// if the article is found in "my" path, this
			// checks to see if the file exists and is readable.
			memset(&article[0], 0, sizeof(article));
			strcpy(article, myArticle);
			strcat(article, buffer);

			found = access(article,F_OK|R_OK);

			// Check to see if the article was found. If not
			// Clear the article buffer and determine
			// if the article is found in "net_class" path, this
			// checks to see if the file exists and is readable.
			if (found != 0)
			{
				memset(&article[0], 0, sizeof(article));
				strcpy(article, netArticle);
				strcat(article, buffer);
			}

			// If the article is found send it to the subscriber.
			if (found == 0)
			{
				// Obtain a handle to the file.
				// TODO need to search directories for the articles.
				file = fopen(article,"r");

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
				close(fd);
	}

	close(fd);
	fclose(file);
	exit (0);
}
