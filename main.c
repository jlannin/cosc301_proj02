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
	struct jobnode *jobs = NULL;
	if(exist < 0)
	{
		printf("%s\n", "shell-config not found, please use full path names");
		search = 0;
	}
	else if (exist == 0)
	{
		
		paths = getPaths();
		paths_print(paths);
	}
	char ** arr;
	char ***commands;
	FILE *datafile = stdin;
	char buffer[1024];
	printf("Shell shell shell:");
	fflush(stdout);
	int check;
	int childreturn;
	int sequential = 1;
	struct pollfd pfd;
	pfd.fd = 0;
	int rv = 0;
	pfd.events = POLLIN;
	pfd.revents = 0; 
	int modetemp = 1;
	while(fgets(buffer, 1024, datafile) != NULL)
	{
		arr = tokenify(buffer);
		commands = extractCommands(arr);	
		if (exist ==0)
		{	
			findFile(&commands, paths);
		}
		runProcesses(commands, (&sequential), &jobs);
		freeToken(arr);
		freeCommands(commands);
		free(arr);
		free(commands);
		if(sequential == 2)
		{
			if (jobs != NULL)
			{
				printf("%s\n", "\nError: There are still processes running.  Unable to exit");
				sequential = modetemp;
			}
			else
			{
				if (exist ==0)
				{
					paths_clear(paths);
			
				}
				jobs_clear(jobs);
				exit(1);
			}
		}
		else
		{
			modetemp = sequential;
		}
		printf("Shell shell shell:");
		fflush(stdout);
		if(sequential == 0)
		{		
			while (1)
			{
				rv = poll(&pfd, 1, 200);
				if (rv == 0)//timeout
				{				
					check = waitpid(0, &childreturn, WNOHANG);
					if (check > 0)
						{
							struct jobnode* child = findchild(check, &jobs);
							if (child != NULL)
							{
								printf("\n\t%s", child->command);
								printf("%s\n", " Finished!");
								printf("%s", "Shell shell shell:");
								fflush(stdout);	
								int delete = jobs_delete(check, &jobs);
								if(!delete)
								{
									printf("%s\n", "Something went terribly, terribly wrong"); //should never get here
								}
							}
							else
							{
								printf("%s\n", "Something went terribly, terribly wrong"); //should never get here
							}
						}
					else if (check == -1)
					{
						if (errno != ECHILD)
						{
							fprintf(stderr, "Wait failed: %s\n", strerror(errno));
						}					
					}
				}
				if (rv > 0)//getinput
				{
					//printf("%s\n", "processing input");
					break;
				}
			}
		}
	}	
	paths_clear(paths);
	jobs_clear(jobs);
    return 0;
}
 
struct jobnode *findchild(pid_t target, struct jobnode **head)
{
	struct jobnode *temp = *head;
	while (temp != NULL)
	{
		if (temp->pid == target)
		{
			return temp;
		}
		temp = temp->next;
	}
	return NULL;
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

void runProcesses(char *** commands, int *sequential, struct jobnode **jobs)
{
	if((*sequential))
		{
			runSequential(commands, sequential, jobs);
		}
		else
		{
			runParallel(commands, sequential, jobs);
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
		paths_append(buffer, &paths);	
	}
	fclose(datafile);
	return paths;
}
