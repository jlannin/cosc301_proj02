#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>

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
	printf("%d\n", commandcount);
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

void runProcesses(char *** commands)
{
	int i = 0;
	while (commands[i] != NULL)
	{
		char **argj = commands[i];
		pid_t pid = fork();
		int childreturn;
		if (pid == 0) //child
		{
			if (execv(argj[0], argj) < 0)
   			{
				fprintf(stderr, "execv failed: %s\n", strerror(errno));
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
		i++;
	}

}

