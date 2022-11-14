/**
 * Various list operations
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "task.h"


// add a new task to the list of tasks
void insert(struct node **head, struct node **tail, Task *newTask) {
    // add the new task to the list 
    struct node *newNode = malloc(sizeof(struct node));
    newNode->task = newTask;
    newNode->next = NULL;
    if (*head == NULL){
        *head = newNode;
        (*head)->prev = NULL;
    }
    else if (*head == *tail){
        (*head)->next = newNode;
        newNode->prev = (*head);
    }
    else {
        (*tail)->next = newNode;
        newNode->prev = *tail;
    }
    *tail = newNode;
}

// delete the selected task from the list
void delete(struct node **head, struct node **tail, Task *task) {
    struct node *temp;
    struct node *p;
    struct node *n;

    temp = *head;
    // special case - beginning of list
    if (strcmp(task->name,temp->task->name) == 0) {
        *head = (*head)->next;
        if (*head != NULL){
            (*head)->prev = NULL;
        }
    }
    else if (strcmp(task->name,(*tail)->task->name) == 0){
        *tail = (*tail)->prev;
        (*tail)->next = NULL;
    }
    else {
        // interior or last element in the list
        temp = temp->next;
        while (strcmp(task->name,temp->task->name) != 0) {
            temp = temp->next;
        }
        p = temp->prev;
        n = temp->next;
        p->next = n;
        n->prev = p;
    }
}

// traverse the list
void traverse(struct node *head) {
    struct node *temp;
    temp = head;
    
    while (temp != NULL) {
        printf("[%s] [%d] [%d]\n",temp->task->name, temp->task->priority, temp->task->burst);
        temp = temp->next;
    }
}
