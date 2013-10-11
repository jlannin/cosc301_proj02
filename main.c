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
	struct stat statresult;
	
	int exist = stat("shell-config", &statresult);
	int search = 1;
	struct node *paths;
	if(exist < 0)
	{
		printf("%s\n", "shell-config not found, please use full path names");
		search = 0;
	}
	else if (exist == 0)
	{
		
		paths = getPaths();
		list_print(paths);
	}
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
		if (exist ==0)
		{	
			findFile(&commands, paths);
		}
		runProcesses(commands, (&sequential));
		freeToken(arr);
		freeCommands(commands);
		free(arr);
		free(commands);
		if(sequential == 2)
		{
			if (exist ==0)
			{
				clear(paths);
			}
			exit(1);
		}

		printf("Shell shell shell:");
	}	
    return 0;
}

void findFile(char **** commands, struct node *list)
{
	int x = 0;
	int valid = 0;
	struct stat statresult;
	char * string;
	char *** temp = *commands;
	char * tempstring;
	struct node *nodetemp = list;
	for(;x < commandCount(*commands); x++)
	{
		tempstring = (temp[x][0]);
		nodetemp = list;
		valid = stat(tempstring, &statresult);
		if (!valid)
		{
			break;
		}
		else
		{
			while (nodetemp != NULL)
			{
				string = strdup(nodetemp->name);
				string = strcat(string, "/");
				tempstring = temp[x][0];
				tempstring = strcat(string,tempstring);
				valid = stat(tempstring, &statresult);
				if (!valid)
				{
					free(temp[x][0]);
					temp[x][0] = tempstring;
					break;
				}	
				nodetemp = nodetemp->next;
				free(string);
			}
	
		}
	}
	commands = &temp;
}

void runProcesses(char *** commands, int *sequential)
{
	if((*sequential))
		{
			runSequential(commands, sequential);
		}
		else
		{
			runParallel(commands, sequential);
		}

}

struct node *getPaths()
{
	FILE *datafile =  NULL;
	datafile = fopen("shell-config", "r");
	if (datafile == NULL)
	{
		printf("Unable to open file %s: %s\n", "shell-config", strerror(errno));
		exit(1);
	}
	
	struct node *paths = NULL;
	char buffer[128];
	while(fgets(buffer, 128, datafile) != NULL)
	{	
		buffer[strlen(buffer)-1] = '\0';
		list_append(buffer, &paths);	
	}
	fclose(datafile);
	return paths;
}
