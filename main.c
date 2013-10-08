#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>
#include "shellper.h"

int main(int argc, char **argv) 
{
	char ** arr;
	char ***commands;
	FILE *datafile = stdin;
	char buffer[1024];
	printf("Shell shell shell:");
	fflush(stdout);
	int sequential = 1;
	while(fgets(buffer, 1024, datafile) != NULL)
	{
		arr = tokenify(buffer);
		commands = extractCommands(arr);
		sequential = runProcesses(commands, (&sequential));
		freeToken(arr);
		freeCommands(commands);
		if(sequential == 2)
		{
			exit(1);
		}

		printf("Shell shell shell:");
	}

    return 0;
}

int runProcesses(char *** commands, int *sequential)
{
	if((*sequential))
		{
			runSequential(commands, sequential);
		}
		else
		{
			(*sequential) = runParallel(commands, sequential);
		}
	return (*sequential);

}
