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
