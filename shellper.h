#ifndef __SHELLPER_H__
#define __SHELLPER_H__

struct node {
	char name[128];
	struct node *next;
};

struct jobnode {
	pid_t pid;
	int running;
	char command[1024];
	struct jobnode *next;
};

void jobs_append(const char *, pid_t, struct jobnode **);
void jobs_print(const struct jobnode *);

char **tokenify(const char *);
char ***extractCommands(char **);
void freeToken(char **);
void freeCommands(char ***);
void runProcesses(char ***, int *, struct jobnode **);
void runSequential(char ***, int *, struct jobnode **);
void modefun(char **, int *);
void runParallel(char ***, int *, struct jobnode **);
int commandCount(char ***);
int *getvalidreturns(char ***);
void paths_append(const char *, struct node **);
void paths_print(const struct node *);
void paths_clear(struct node *);
void findFile(char ****, struct node *);
struct node *getPaths();
void jobs_clear(struct jobnode *);
struct jobnode *findchild(pid_t, struct jobnode **);
int jobs_delete(pid_t, struct jobnode **);

#endif // __SHELLPER_H__
