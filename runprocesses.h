#ifndef __RUNPROCESSES_H__
#define __RUNPROCESSES_H__
#include "jobnode.h"

void runProcesses(char ***, int *, struct jobnode **);
void runSequential(char ***, int *, struct jobnode **);
void modefun(char **, int *);
void runParallel(char ***, int *, struct jobnode **);
int commandCount(char ***);
void runpause(char ***, struct jobnode **, int);
void resume(char ***, struct jobnode **, int);

#endif // __RUNPROCESSES_H__
