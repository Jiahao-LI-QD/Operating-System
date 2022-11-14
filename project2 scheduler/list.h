/**
 * list data structure containing the tasks in the system
 */

#include "task.h"

struct node {
    Task *task;
    struct node *prev;
    struct node *next;
};

// insert and delete operations.
void insert(struct node **head, struct node **tail, Task *task);
void delete(struct node **head, struct node **tail, Task *task);
void traverse(struct node *head);
//struct node *search(int i);
