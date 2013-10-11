#ifndef __SHELLPER_H__
#define __SHELLPER_H__
#include "jobnode.h"

struct node {
	char name[128];
	struct node *next;
};

char **tokenify(const char *);
char ***extractCommands(char **);
void freeToken(char **);
void freeCommands(char ***);
void runProcesses(char ***, int *, struct jobnode **);
void runSequential(char ***, int *, struct jobnode **);
void modefun(char **, int *);
void runParallel(char ***, int *, struct jobnode **);
int commandCount(char ***);
void paths_append(const char *, struct node **);
void paths_print(const struct node *);
void paths_clear(struct node *);
void findFile(char ****, struct node *);
struct node *getPaths();
void jobs_clear(struct jobnode *);
struct jobnode *findchild(pid_t, struct jobnode **);
int jobs_delete(pid_t, struct jobnode **);
char *ourconcat(char *, char *, char *);

#endif // __SHELLPER_H__
