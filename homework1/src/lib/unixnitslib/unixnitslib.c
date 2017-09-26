#include <stdio.h>
#include "unixnitslib.h"

int setup_subscriber(char *publisher_path)
{
	printf ("Setting up unix domain subscriber on %s\n", publisher_path);
	return (0);
}

int setup_publisher(char *publisher_path)
{
	printf ("Setting up unix domain publisher on %s\n", publisher_path);
	return (0);
}
