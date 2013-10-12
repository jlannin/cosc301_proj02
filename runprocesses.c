#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shellper.h"
#include "jobnode.h"
#include "paths.h"
#include "runprocesses.h"

/*
This file contains basically everything needed to run the processes.
It contains methods for sequential, parallel, and some helper files too.
*/

/*
This runs everything that needs to happen for sequential
*/
void runSequential(char *** commands, int *sequential, struct jobnode **jobs)
{
	int i = 0;
	while (commands[i] != NULL)
	{
		char **argj = commands[i];
		if (strcmp(commands[i][0], "exit") == 0)
		{
			if (commands[i][1] == NULL)
			{
				(*sequential) = 2;
			}
			else
			{
				printf("%s\n", "Error: \"exit\" has no parameters!");
			}
		}
		else if (strcmp(commands[i][0], "mode") == 0)
		{
			modefun(commands[i], sequential);
		}
		else if (strcmp(commands[i][0], "jobs") == 0)
		{
			if (commands[i][1] == NULL)
			{
				jobs_print(*jobs);
			}
			else
			{
				printf("%s\n", "Error: \"jobs\" has no parameters!");
			}
		}
		else
		{
			pid_t pid = fork();
			int childreturn;
			if (pid == 0) //child
			{
				if (execv(argj[0], argj) < 0)
   				{
					printf("Execv of \"%s\" failed: %s\n", argj[0], strerror(errno));
					exit(1);
				}
			}
			else if (pid > 0)
			{
     				int check;
     				check = waitpid(pid, &childreturn, 0);
       				if (check == -1)
				{
					fprintf(stderr, "Wait failed: %s\n", strerror(errno));
					exit(1);

				}
    			}
			else
			{
				fprintf(stderr, "Fork failed: %s\n", strerror(errno));
				exit(1);
			}
		}
		i++;
	}
}

/*
Runs parallel.  Commands has the commands in it, sequential the mode, and
jobs, the total number of jobs. This function uses modefun, runpause, and
resume helper functions.
*/
void runParallel(char ***commands, int *sequential, struct jobnode **jobs)
{
	int i = 0;
	while(commands[i] != NULL)
	{
		char **argj = commands[i];
		if (strcmp(commands[i][0], "exit") == 0)
		{
			if (commands[i][1] == NULL)
			{
				(*sequential) = 2;
			}
			else
			{
				printf("%s\n", "Error: \"exit\" has no parameters!");
			}
		}
		else if (strcmp(commands[i][0], "mode") == 0)
		{
			modefun(commands[i], sequential);
		}
		else if (strcmp(commands[i][0], "jobs") == 0)
		{
			
			if (commands[i][1] != NULL)
			{
				printf("%s\n", "Error: \"jobs\" has no parameters!");
			}
				jobs_print(*jobs);
		}
		else if (strcmp(commands[i][0], "pause") == 0)
		{
			runpause(commands,jobs, i);
		}
		else if (strcmp(commands[i][0], "resume") == 0)
		{
			resume(commands, jobs, i);	
		}
		else
		{	
			pid_t pid1 = fork();
			if (pid1 == 0)
			{
				if (execv(argj[0], argj) < 0)
				{
					printf("Execv of \"%s\" failed: %s\n", argj[0], strerror(errno));
					exit(1);
				}
			}
			else if (pid1 > 0)
			{
				jobs_append(commands[i], pid1, jobs);
			}
			else
			{
				fprintf(stderr, "Fork failed: %s\n", strerror(errno));
				exit(1);
			}
		}
	i++;
	}
}

/*
Handles the pause command.
*/
void runpause(char *** commands, struct jobnode **jobs, int i)
{
	if (commands[i][1] == NULL)
	{
		printf("%s\n", "Error: \"pause\" needs one parameter!");
	}
	else if (commands[i][2] != NULL)
	{
		printf("%s\n", "Error: \"pause\" has only one parameter!");
	}
	else
	{
		int pausetarget = atol(commands[i][1]); 
		struct jobnode *paused = findchild(pausetarget, jobs);
		if (paused == NULL) //can't find child
		{
			printf("%s\n", "Error: In \"pause,\" unable to find process!");
		}
		else
		{
			if (paused->running == 0)
			{
				printf("%s\n", "Error: Process already paused!");
			}
			else
			{
			int killresult = kill(pausetarget, SIGSTOP);
			if (killresult == 0)
			{
				paused->running = 0;
			}					
			else if(killresult < 0)
				{
					printf("Pause failed: %s\n", strerror(errno));
					exit(1);
				}	
			}
		}
	}
}

/*
Hands the resume command.
*/
void resume(char *** commands, struct jobnode **jobs, int i)
{
	if (commands[i][1] == NULL)
	{
		printf("%s\n", "Error: \"resume\" needs one parameter!");
	}
	else if (commands[i][2] != NULL)
	{
		printf("%s\n", "Error: \"resume\" has only one parameter!");
	}
	else
	{
		int resumetarget = atol(commands[i][1]); 
		struct jobnode *resume = findchild(resumetarget, jobs);
		if (resume == NULL) //can't find child
		{
			printf("%s\n", "Error: In \"resume,\" unable to find process!");
		}
		else
		{
			if (resume->running == 1)
			{
				printf("%s\n", "Error: Process already running!");
			}
			else
			{
				int killresult = kill(resumetarget, SIGCONT);
				if (killresult == 0)
				{
					resume->running = 1;
				}		
				else if(killresult < 0)
				{
					printf("Resume failed: %s\n", strerror(errno));
					exit(1);
				}			
			}
		}
	}

}

/*
Handles the mode command.
*/
void modefun(char **commands, int *sequential)
{
	if((*sequential) != 2)
	{
		if(commands[1] == NULL)
		{
			if((*sequential))
			{
				printf("%s\n","sequential mode");
			}
			else
			{
				printf("%s\n", "parallel mode");
			}
		}
		else if(commands[2] == NULL)
		{
			if(strncasecmp("parallel", commands[1], strlen(commands[1])) == 0)
			{
				(*sequential) = 0;
			}
			else if(strncasecmp("sequential",commands[1], strlen(commands[1])) == 0)
			{
				(*sequential) = 1;
			}
			else
			{
				fprintf(stderr, "%s\n", "Incorrect mode specified, please only use parallel or sequential. Thank you.");
			}
		}
		else
		{
			fprintf(stderr, "%s\n", "Too many parameters, mode only allows one.");
		}
	}
}

/*
Called from main, determines which mode we are running in.
*/
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

/*
Returns number of commands.
*/
int commandCount(char ***commands)
{
	int i = 0;
	while (commands[i] != NULL)
	{
		i++;
	}
	return i;
}




