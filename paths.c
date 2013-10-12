#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "paths.h"
/*
paths.c and paths.h basically define everything that we need for the path
part of our program.  It tries to read in shell-config" and if successful
makes a linked list with all of the paths as nodes.
*/


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
	int success = fclose(datafile);
	if (success)
	{
		printf("Unable to close file %s: %s\n", "shell-config", strerror(errno));
		exit(1);
	}	
	return paths;
}


void paths_append(const char *name, struct node **head) {
	if (name == NULL)
	{
		return;
	}
	struct node *newnode = (struct node*) malloc(sizeof(struct node));
	if(newnode==NULL)
	{
		printf("%s\n", "Malloc Fails: Exiting Now!");
		exit(1);
	}	
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
