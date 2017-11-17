/*
 *	<Header stuff goes here>
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "nitslib.h"
#define MAXLEN 1024
#define SUB_BUFLEN 80
#define ART_BUFLEN 1024

#define BUFFER_SIZE 1024
#define DEFAULT_PORT	"localhost:8404"

// Path's to look for the articles
char myArticle[] = "/home/eflores4/Articles/";
//char netArticle[] = "/home/edwin/";
char netArticle[] = "/home/net_class/474/Articles/";

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

		if (bytes <= 0)
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
			//kill(getppid(), SIGKILL);
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

int main(int argc, char *argv[])
{
	int c;
	//char publisher[MAXLEN];
	//char discovery[MAXLEN];
	char *phost, *pport;
	char *dhost, *dport;
	int fd;

	char *publisher = NULL;
	char *discovery = NULL;

	while ((c = getopt(argc, argv, "hd:")) != -1)
	{
		switch (c)
		{
				case 'd':
				discovery = optarg;
				break;
				case 'p':
				publisher = optarg;
				default:
				fprintf (stderr, "Usage: %s -d <host:port>\n", argv[0]);
				exit (1);
		}
	}
	if (discovery == NULL)
	{
		discovery = malloc(MAXLEN);
		strcpy (discovery, DEFAULT_PORT);
	}

	if (publisher == NULL)
	{
		publisher = malloc(MAXLEN);
		strcpy (publisher, DEFAULT_PORT);
	}

	get_host_and_port (discovery, &dhost, &dport);
	get_host_and_port (publisher, &phost, &pport);

	printf("P: %s, %s D: %s, %s\n", phost, pport, dhost, dport);

	register_publisher(phost, pport, dhost, dport);

	if ((setup_publisher (phost, pport)) == NITS_SOCKET_ERROR)
	{
		fprintf (stderr, "Cannot set up publisher.\n");
		exit(1);
	}

	fd = get_next_subscriber();

	if (fd == NITS_SOCKET_ERROR)
	{
		printf("ERROR SETUP");
		exit(1);
	}

	child_process(fd);

	exit (0);
}
