#ifndef __SHELLPER_H__
#define __SHELLPER_H__

char **tokenify(const char *);
char ***extractCommands(char **);
void freeToken(char **);
void freeCommands(char ***);
int runProcesses(char ***, int *);
void runSequential(char ***, int *);
void modefun(char **, int *);
int runParallel(char ***, int *);
int commandCount(char ***);

#endif // __SHELLPER_H__
