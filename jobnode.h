#ifndef __JOBNODE_H__
#define __JOBNODE_H__

struct jobnode {
	pid_t pid;
	int running;
	char command[1024];
	struct jobnode *next;
};

void jobs_append(const char *, pid_t, struct jobnode **);
void jobs_print(const struct jobnode *);
struct jobnode *findchild(pid_t, struct jobnode **);
void jobs_clear(struct jobnode *);
int jobs_delete(pid_t, struct jobnode **);

#endif // __PATHS_H__
