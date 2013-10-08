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

//break up input string into sections by semicolon
char **tokenify(const char *str)
{
	int j = 0;
	char *tmp, *word;
	const char *sep = ";";
	char *s = strdup(str);
	int count = 0;
	for(; j < strlen(s); j++)
	{
		if (s[j] == '#')
		{
			s[j] = '\0';
		}
	}
	for (word = strtok_r(s, sep, &tmp); word != NULL; word = strtok_r(NULL, sep, &tmp))
	{
		count++;
	}
	char **array = (char**) malloc((sizeof(char*))*(count + 1));
	free(s);
	count = 0;
	char *s1 = strdup(str);
	j = 0;
	for(; j < strlen(s1); j++)
	{
		if (s1[j] == '#')
		{
			s1[j] = '\0';
		}
	}
	for (word = strtok_r(s1, sep, &tmp); word != NULL; word = strtok_r(NULL, sep, &tmp))
        {
              	array[count] = strdup(word);
                count++;
        }
	array[count] = NULL;
	free(s1);
	return array;
}

//words broken up by semicolons
char ***extractCommands(char **token)
{
	int i = 0;
	int commandcount = 0;
	int count;
	while (token[i] != NULL)
	{
		char *tmp, *word;
		const char *sep = " \n\t";
		char *s = strdup(token[i]);
		count = 0;
		for (word = strtok_r(s, sep, &tmp); word != NULL; word = strtok_r(NULL, sep, &tmp))
		{
			count++;
		}
		if(count > 0)
		{
			commandcount++;
		}
		free(s);
	i++;
	}
	//printf("%d\n", commandcount);
	//i should be number of commands
	char ***commands = (char***) malloc((sizeof(char**))*(commandcount+1));
	i = 0;
	count = 0;
	while (token[i] != NULL)
	{
		char *tmp, *word;
		const char *sep = " \n\t";
		char *s1 = strdup(token[i]);
		count = 0;
		for (word = strtok_r(s1, sep, &tmp); word != NULL; word = strtok_r(NULL, sep, &tmp))
		{
			count++;
		}
		if (count > 0)
		{
			char **com = (char**) malloc((sizeof(char*))*(count+1));
			free(s1);
			char *s2 = strdup(token[i]);
			count = 0;
			for (word = strtok_r(s2, sep, &tmp); word != NULL; word = strtok_r(NULL, sep, &tmp))
	        	{
	              		com[count] = strdup(word);
	               		count++;
		       	}
			com[count] = NULL;
			commands[i] = com;
			free(s2);
		}
		i++;
	}
	commands[commandcount] = NULL;
	return commands;
}

void freeToken(char **arr)
{
	int i = 0;
	while (arr[i] != NULL)
		{
			//printf(arr[i]);
			free(arr[i]);
			i++;
		}
		free(arr);
}

void freeCommands(char ***arr)
{
	int i = 0;
	while (arr[i] != NULL)
	{
		freeToken(arr[i]);
	i++;
	}
	free(arr);
}

void runSequential(char *** commands, int *sequential)
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
		}
		else if (strcmp(commands[i][0], "mode") == 0)
		{
			modefun(commands[i], sequential);
		}
		else
		{
			pid_t pid = fork();
			int childreturn;
			if (pid == 0) //child
			{
				if (execv(argj[0], argj) < 0)
   				{
					fprintf(stderr, "execv failed: %s\n", strerror(errno));
					exit(1);
				}
			}
			else if (pid > 0)
			{
     				int check;
     				check = waitpid(pid, &childreturn, 0);
       				if (check != -1)
				{
   			   		printf("%s\n", "Child process finished");
        			}
				else
				{
					fprintf(stderr, "Wait failed: %s\n", strerror(errno));
				}
    			}
		}
		i++;
	}
}

int runParallel(char ***commands, int *sequential)
{
	pid_t pid = fork();
	int childreturn;
	int i = 0;
	int numCommands = commandCount(commands);
	int check;
	if (pid == 0)
	{
		while(commands[i] != NULL)
		{
			pid_t pid1 = fork();
			if (pid1 == 0)
			{
				char **argj = commands[i];
				if (strcmp(commands[i][0], "exit") == 0)
				{
					if (commands[i][1] == NULL)
					{
						printf("REACHED");
						exit(2);
					}
				}
				else if (strcmp(commands[i][0], "mode") == 0)
				{
					modefun(commands[i], sequential);
					exit(*sequential);
				}
				else
				{
					if (execv(argj[0], argj) < 0)
	   				{
						fprintf(stderr, "execv failed: %s\n", strerror(errno));
						exit(*sequential);
					}
				}
			}
			else if (pid1 > 0)
			{
				// if returnarr[i] = 1 
				// then outputarry [i] = 
				i++;
			}
		}
		i = 0;
		while (i < numCommands)
		{
			check = wait(&childreturn);
			printf("%d\n", WEXITSTATUS(childreturn));
			if((*sequential) != 2)
			{
				(*sequential) = WEXITSTATUS(childreturn);
			}
			if (check == -1)
			{
				fprintf(stderr, "Wait failed HERE: %s\n", strerror(errno));
				//exit(1);
			}
		i++;
		}
		exit((*sequential));
	}
	else if (pid > 0)
	{
		
		check = waitpid(pid,&childreturn,0);
		printf("%d\n", WEXITSTATUS(childreturn));
		if((*sequential) != 2)
		{
			(*sequential) = WEXITSTATUS(childreturn);
		}
		if (check == -1)	
		{
			fprintf(stderr, "Wait failed: %s\n", strerror(errno));
			exit(1);
		}
	}
	return (*sequential);
}

int commandCount(char ***commands)
{
	int i = 0;
	while (commands[i] != NULL)
	{
		i++;
	}
	return i;
}


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

