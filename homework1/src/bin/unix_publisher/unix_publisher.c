/*
 *	<Header stuff goes here>
 */
#include <stdlib.h>
#include <stdio.h>
#include "unixnitslib.h"

int
main(int argc, char *argv[])
{
	int fd = 0;
	char buffer[256];
	
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

	// Obtain the file descriptor. Exit is there is an error.
	fd = get_next_subscriber();

	if (fd == NITS_SOCKET_ERROR)
	{
		perror("RROR");
		exit(1);
	}

	// Read whcih article the publisher requested.
	int n = read(fd, buffer, 255);

	if (n < 0)
		printf("ERROR READING");


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

	close(fd);
	fclose(file);
	exit (0);
}
