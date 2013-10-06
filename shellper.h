#ifndef __SHELLPER_H__
#define __SHELLPER_H__

char **tokenify(const char *);
char ***extractCommands(char **);
void freeToken(char **);
void freeCommands(char ***);
void runProcesses(char ***);

#endif // __SHELLPER_H__
