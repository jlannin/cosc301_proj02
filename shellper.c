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
	int added = 0;
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
		free(s1);
		if (count > 0)
		{
			char **com = (char**) malloc((sizeof(char*))*(count+1));
			char *s2 = strdup(token[i]);			
			count = 0;
			for (word = strtok_r(s2, sep, &tmp); word != NULL; word = strtok_r(NULL, sep, &tmp))
	        	{
	              		com[count] = strdup(word);
	               		count++;
		       	}	
			com[count] = NULL;
			commands[added] = com;
			added++;
			free(s2);
		}
		i++;
	}
	commands[added] = NULL;
	return commands;
}

void freeToken(char **arr)
{
	int i = 0;
	while (arr[i] != NULL)
		{
			free(arr[i]);
			i++;
		}
}

void freeCommands(char ***arr)
{
	int i = 0;
	while (arr[i] != NULL)
	{
		freeToken(arr[i]);
		free(arr[i]);
	i++;
	}
}

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
				}
    			}
		}
		i++;
	}
}

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
		else if (strcmp(commands[i][0], "resume") == 0)
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
				jobs_append(commands[i][0], pid1, jobs);
			}
		}
	i++;
	}
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


void paths_append(const char *name, struct node **head) {
	if (name == NULL)
	{
		return;
	}
	struct node *newnode = (struct node*) malloc(sizeof(struct node));
	strncpy((newnode->name), name, 127);
	newnode->next = NULL;
 	while((*head) != NULL)
       	{
       		head = &((*head)->next);
	}
	(*head)=newnode;
}

void paths_print(const struct node *head) {
        while(head != NULL)
        {
                printf("Node at address %p has value %s\n", head, head->name);
                head = head->next;
        }
}

void paths_clear(struct node *list)
{
	while(list != NULL)
		{
			struct node *temp = list;
			list = list->next;
			free(temp);
		}
}


