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
Shellper contains a variety of files which
help our shell, including the functions which
parse the input from the user and a couple of free
functions.
*/


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


/*
Tokenify breaks the user input into an
array of strings, removing any comments along the way.
*/
char **tokenify(const char *str)
{
	int j = 0;
	char *tmp, *word;
	const char *sep = ";";
	char *s = strdup(str);
	if (s==NULL)
	{
		printf("Strdup Fails: %s\n", strerror(errno));
		exit(1);
	}
	int count = 0;
	for(; j < strlen(s); j++)
	{
		if (s[j] == '#')
		{
			s[j] = '\0';
		}
	}
	//first run through to get number of separate commands
	for (word = strtok_r(s, sep, &tmp); word != NULL; word = strtok_r(NULL, sep, &tmp))
	{
		count++;
	}
	char **array = (char**) malloc((sizeof(char*))*(count + 1));
	if(array==NULL)
	{
		printf("%s\n", "Malloc Fails: Exiting Now!");
		exit(1);
	}	
	free(s);
	count = 0;
	char *s1 = strdup(str);
	if (s1==NULL)
	{
		printf("Strdup Fails: %s\n", strerror(errno));
		exit(1);
	}
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
		if (array[count]==NULL)
		{
			printf("Strdup Fails: %s\n", strerror(errno));
			exit(1);
		}
                count++;
        }
	array[count] = NULL;
	free(s1);
	return array;
}

/*
Breaks each unique command (previously separated by tokenify)
up by space/tab/newline and puts them into a larger array of string arrays.
	
*/
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
		if (s==NULL)
		{
			printf("Strdup Fails: %s\n", strerror(errno));
			exit(1);
		}
		count = 0;
		for (word = strtok_r(s, sep, &tmp); word != NULL; word = strtok_r(NULL, sep, &tmp))
		{
			count++;	
		}
		//count records how many actual words there are (not just blank spaces) in each "command"
		//if there is an actual word, that section is counted as a real command and the command count increases
		if(count > 0)
		{
			commandcount++;
		}
		free(s);
	i++;
	}
	char ***commands = (char***) malloc((sizeof(char**))*(commandcount+1));
	if(commands==NULL)
	{
		printf("%s\n", "Malloc Fails: Exiting Now!");
		exit(1);
	}
	i = 0;
	count = 0;
	while (token[i] != NULL)
	{	
		char *tmp, *word;
		const char *sep = " \n\t";
		char *s1 = strdup(token[i]);
		if (s1==NULL)
		{
			printf("Strdup Fails: %s\n", strerror(errno));
			exit(1);
		}
		count = 0;
		for (word = strtok_r(s1, sep, &tmp); word != NULL; word = strtok_r(NULL, sep, &tmp))
		{
			
			count++;
		}
		free(s1);
		if (count > 0)
		{
			char **com = (char**) malloc((sizeof(char*))*(count+1));
			if(com==NULL)
			{
				printf("%s\n", "Malloc Fails: Exiting Now!");
				exit(1);
			}
			char *s2 = strdup(token[i]);
			if (s2==NULL)
			{
				printf("Strdup Fails: %s\n", strerror(errno));
				exit(1);
			}			
			count = 0;
			for (word = strtok_r(s2, sep, &tmp); word != NULL; word = strtok_r(NULL, sep, &tmp))
	        	{
	              		com[count] = strdup(word);
				if (com[count]==NULL)
				{
					printf("Strdup Fails: %s\n", strerror(errno));
					exit(1);
				}
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






