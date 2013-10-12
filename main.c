/**
Justin Lannin and Adam Weber
10/11/13

We both coded together in order to catch errors and talk about ideas.  Because of this, it is a little hard to say who exactly
was responsible for what, we both made significant contributions.



The basic structure of our program is as follows:

Read in shell-config
	-store paths in variable "paths"
Read in input:
Parse input by semicolons
Parse parsed input by spaces
	-Commands are now stored in "commands" which is a char***
	-each commands[i] is a single command so that commands[i][0] is the
	-process name.
Run sequential or parallel depending.
Check for exiting
Free what needs to be freed.
If in parallel mode, call preparellel which prepares
the shell for parallel mode (switching back and forth
between waiting for processes and user input)


*/




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
#include "jobnode.h"
#include "paths.h"
#include "runprocesses.h"
 
/*
A quick concat function that we wrote for using paths.
*/
char *ourconcat(char * begin, char * mid, char *end)
{
	int len1 = strlen(begin) + strlen(mid) + strlen(end) + 1;
	char * newstring = (char *) malloc(sizeof(char)*len1);
	int x = 0;
	int numbeg = strlen(begin);
	for (; x < numbeg; x++)
	{
		newstring[x] = begin[x];
	}
	x = 0;
	for (; x < strlen(mid); x++)
	{
		newstring[x + numbeg] = mid[x];
	}
	x = 0;
	for (; x < strlen(end); x++)
	{
		newstring[x+numbeg+strlen(mid)] = end[x];
	}
	newstring[len1-1] = '\0';
	return newstring;

}


/*
Preparellel is called from main and is used when the shell is in
parallel mode and is waiting for input.  It will
switch back and forth between checking for user input
and waiting for processes to finish.  If a process 
finishes, it will print out a message that the process finished.
*/
void preparellel(struct jobnode **jobs)
{
	int check = 0;
	int rv = 0;
	int childreturn;		
	struct pollfd pfd;
	pfd.fd = 0;
	pfd.events = POLLIN;
	pfd.revents = 0; 
	while (1)
	{
		rv = poll(&pfd, 1, 200);
		if (rv == 0)//timeout
		{				
			check = waitpid(0, &childreturn, WNOHANG);
			if (check > 0)
				{
					struct jobnode* child = findchild(check, jobs);
					if (child != NULL)
					{
						printf("%s", "Process ");
						printf("%d", child->pid);
						printf("%s", " (");
						jobs_commandprint(child->command, 0);
						printf("%s", ") ");
						printf("%s\n", "completed");
						printf("%s", "Shell shell shell:");
						int flush = fflush(stdout);
						if (flush)
						{
							fprintf(stderr, "Flush failed: %s\n", strerror(errno));
						}
						int delete = jobs_delete(check, jobs);
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
		else if (rv > 0)//getinput
		{
			break;
		}
		else
		{
			fprintf(stderr, "Poll Failed: %s\n", strerror(errno));
			exit(1);
		}
		
	}

}
/*
void printcommand(char ** command)
{
	while(command != NULL
}
*/
/*
Using the paths stored in list, it looks for the commands
and replaces those commands that don't exist with ones that do.
If no existing files are found, the command is left to fail as is
with execv later on.
*/
void findFile(char **** commands, struct node *list)
{
	int x = 0;
	int valid = 0;
	struct stat statresult;
	char * string;
	char *** temp = *commands;
	struct node *nodetemp = list;
	for(;x < commandCount(*commands); x++)
	{
		string = temp[x][0];
		nodetemp = list;
		valid = stat(string, &statresult);
		if (!valid)
		{
			break;
		}	
		else
		{
			while (nodetemp != NULL)
			{
				string = ourconcat((nodetemp->name), "/", temp[x][0]);
				valid = stat(string, &statresult);				
				if (valid == 0)
				{
					free(temp[x][0]);
					temp[x][0] = string;
					break;
				}	
				nodetemp = nodetemp->next;
				free(string);
			}
	
		}
	}
	commands = &temp;
}

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
	}
	char ** arr;
	char ***commands;
	FILE *datafile = stdin;
	char buffer[1024];
	printf("Shell shell shell:");
	int flush = fflush(stdout);
	if (flush)	
	{	
		fprintf(stderr, "Flush failed: %s\n", strerror(errno));
	}
	int sequential = 1;
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
		flush = fflush(stdout);
		if (flush)	
		{	
			fprintf(stderr, "Flush failed: %s\n", strerror(errno));
		}
		if(sequential == 0)
		{		
			preparellel(&jobs);
		}
	}	
	paths_clear(paths);
	jobs_clear(jobs);
    return 0;
}
