/*
 *	<Header stuff goes here>
 */
#include <stdlib.h>
#include <stdio.h>
#include "unixnitslib.h"

#define ARRAY_SIZE 256

int main(int argc, char *argv[])
{
	int fd;
	int bytes;
	char buffer[ARRAY_SIZE];

	/*
	 * check args.
	 */
	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <source file>\n", argv[0]);
		exit (1);
	}
	/*
	 * do your stuff here...just calling setup_publisher
	 * in this example for fun.
	 */


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
			int bytes = read(fd, buffer, ARRAY_SIZE-1);

			if (bytes < 0)
				perror("Error Reading");

			// Check to see if QUIT was received,
			// If so break from the while loop
			if (stncmp(buffer,"QUIT",4) == 0)
				break;


			// Obtain a handle to the file.
			// TODO need to search directories for the articles.
			FILE *file;
			file = fopen(buffer,"r");

			if (file == NULL)
			{
				perror("Error Opening File");
				exit(1);
			}

			while(fgets(buffer, sizeof(buffer),file))
				write(fd,buffer,255);
	}

	close(fd);
	fclose(file);
	exit (0);
}
