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


void jobs_append(const char *newcommand, pid_t newpid, struct jobnode **head) {
	if (newcommand == NULL)
	{
		return;
	}
	struct jobnode *newnode = (struct jobnode*) malloc(sizeof(struct jobnode));
	strncpy((newnode->command), newcommand, 1023);
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
                printf("%s\n", head->command);
		fflush(stdout);
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
			free(temp);
		}
}

