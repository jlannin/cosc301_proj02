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

int main(int argc, char **argv) {
    printf("main.c, so lonely.  no code here yet :-(\n");
    char *argj[] = { "/bin/ls", "-l", "-r", "-t",  NULL };
  //  if (execv(argj[0], argj) < 0)
    {
	//fprintf(stderr, "execv failed: %s\n", strerror(errno));
    }
//testing
	char ** arr;
	char ***commands;
	FILE *datafile = stdin;
	char buffer[1024];
	printf("Shell shell shell:");
	fflush(stdout);
	int i;
	int j;
	int **sequential;
	**sequential = 1;
	while(fgets(buffer, 1024, datafile) != NULL)
	{
		arr = tokenify(buffer);
		commands = extractCommands(arr);
		i = 0;
		/*while(commands[i] != NULL)
		{
			j = 0;
			while(commands[i][j] != NULL)
			{
				printf("%s\n", commands[i][j]);
				j++;
			}
			printf("\n");
			i++;
		}
		*/
		if(sequential)
		{
			runProcesses(commands, sequential);
			printf("%d\n", sequential);
		}
		else
		{
			runParallel(commands, sequential);
		}
		freeToken(arr);
		freeCommands(commands);
		if(**sequential == 2)
		{
			exit(1);
		}

		printf("Shell shell shell:");
	}

    return 0;
}

