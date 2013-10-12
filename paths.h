#ifndef __PATHS_H__
#define __PATHS_H__



struct node {
	char name[128];
	struct node *next;
};

struct node *getPaths();
void paths_append(const char *, struct node **);
void paths_print(const struct node *);
void paths_clear(struct node *);

#endif // __PATHS_H__
