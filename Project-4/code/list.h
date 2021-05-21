/**
 * list data structure containing the tasks in the system
 */

#include "task.h"

struct node {
    Task *task;
    struct node *next;
    int last_end_execution;// special variable for rr, this variable and task is bonding!!!
};

// insert and delete operations.
void insert(struct node **head_first_in_head, Task *task);
void delete(struct node **head_first_in_head, Task *task);
void traverse(struct node *head);
