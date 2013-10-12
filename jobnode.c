#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "jobnode.h"
#include "shellper.h"

/*
Contains a variety of functions which define a struct jobnode. 
A linkedlist like node which contains all of the information for a
process.
*/

void jobs_append(char **newcommand, pid_t newpid, struct jobnode **head) {
	if (newcommand == NULL)
	{
		return;
	}
	struct jobnode *newnode = (struct jobnode*) malloc(sizeof(struct jobnode));
	if(newnode==NULL)
	{
		printf("%s\n", "Malloc Fails: Exiting Now!");
		exit(1);
	}	
	int count =0;
	while(newcommand[count]!= NULL)
	{
		count++;
	}
	(newnode->command) = (char **) malloc(sizeof(char**)*(count+1));
	char **temp = newnode->command;
	int x = 0;	
	while(newcommand[x] != NULL)
	{
		temp[x] = strdup(newcommand[x]);
		if (temp[x] == NULL)
		{
			printf("Strdup Fails: %s\n", strerror(errno));
			exit(1);
		}
		x++;
	}
	
	temp[x] = NULL;
	newnode->pid = newpid;
	newnode->running = 1;
	newnode->next = NULL;
 	while((*head) != NULL)
       	{
		head = &((*head)->next);
	}
	(*head)=newnode;

}

int jobs_delete(pid_t target, struct jobnode **head) {
	while((*head) != NULL)
        {
		if((*head)->pid== target)
        	{
			struct jobnode *temp = (*head);
			(*head) = (*head)->next;
			freeToken(temp->command);
			free(temp);
			return 1;
               	}
		head = &((*head)->next);
       	}
	return 0;
}

void jobs_print(const struct jobnode *head) {

	if (head == NULL)
	{
		printf("%s\n", "\nNo processes currently running");
		return;
	}     
	printf("%s\n","");
	printf("%s\n", "\tProcess Id\tState\t\tCommand");
	while(head != NULL)
        {
		printf("%s\t", "");
                printf("%d", head->pid);
		printf("%s\t\t", "");
		if (head->running)
		{
			printf("%s", "Running\t\t");
		}
		else
		{
			printf("%s", "Paused\t\t");
		}
              	jobs_commandprint(head->command, 1);
		int flush = fflush(stdout);
		if (flush)	
		{	
			fprintf(stderr, "Flush failed: %s\n", strerror(errno));
		}
                head = head->next;
        }
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

void jobs_clear(struct jobnode *list)
{
	while(list != NULL)
		{
			struct jobnode *temp = list;
			list = list->next;
			freeToken(temp->command);
			free(temp);
		}
}

void jobs_commandprint(char **commands, int newline)
{
	int x =0;
	if(commands==NULL)
	{
		printf("%s\n", "Something went terribly, terribly wrong"); //should never get here
		return;
	}
	if(commands[x]!=NULL)
	{
		printf("%s",commands[x]);
		x++;	
	}	
	while (commands[x]!=NULL)
	{	
		printf(" %s", commands[x]);
		x++;
	}
	if (newline)
	{
		printf("%s\n", "");
	}
}
