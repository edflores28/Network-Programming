#include <stdio.h>
#include "utillib.h"

void parse_arg(int argc, char *argv[], char* disc_addr,)
{
	/* The valid switches that this looks for is -d */
	int idx = argc - 1;
  int i;

	disc_addr = NULL;

	if ((idx % 2 != 0) || idx == 0)
	{
		printf("Invalid or no arguments.\n");
		printf("The following are valid switches:\n");
		printf("-d 1\t\tAddress of discovery server\n");
		return;
	}

	for (int i = 1; i < idx; i+=2)
	{
		char *temp = argv[i];
		if ((temp[0] == '-') && (temp[1] == 'd'))
		{
				disc_addr = argv[i+1];
		}
}