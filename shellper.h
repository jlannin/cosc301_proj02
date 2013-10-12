#ifndef __SHELLPER_H__
#define __SHELLPER_H__
#include "jobnode.h"



char **tokenify(const char *);
char ***extractCommands(char **);
void freeToken(char **);
void freeCommands(char ***);

#endif // __SHELLPER_H__
