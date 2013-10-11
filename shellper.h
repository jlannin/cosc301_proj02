#ifndef __SHELLPER_H__
#define __SHELLPER_H__

struct node {
	char name[128];
	struct node *next;
};

char **tokenify(const char *);
char ***extractCommands(char **);
void freeToken(char **);
void freeCommands(char ***);
void runProcesses(char ***, int *);
void runSequential(char ***, int *);
void modefun(char **, int *);
void runParallel(char ***, int *);
int commandCount(char ***);
int *getvalidreturns(char ***);
void list_append(const char *, struct node **);
void list_print(const struct node *);
void clear(struct node *);
void findFile(char ****, struct node *);
struct node *getPaths();


#endif // __SHELLPER_H__
