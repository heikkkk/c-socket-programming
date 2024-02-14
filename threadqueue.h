#ifndef THREADQUEUE_H
#define THREADQUEUE_H
	
#include <stdio.h>

struct node {
	struct node* next;
	int *client_socket;
};
typedef struct node node_t;

void enqueue(int *client_socket);
int* dequeue();

#endif
