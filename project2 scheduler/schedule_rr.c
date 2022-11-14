#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

#define quantum 10

struct node **head;
struct node **tail;
int count = 0;
int waiting = 0;
int turnaround = 0;
int response = 0;

void add(char *name, int priority, int burst){
	struct task *newTask = malloc(sizeof(struct task));
	if (head == NULL){
		head = malloc(sizeof(head));
		tail = malloc(sizeof(head));
		(*head)= NULL;
		(*tail)= NULL;
	}
	
	newTask->name = name;
	newTask->tid = count++;
	newTask->priority = priority;
	newTask->burst = burst;
	newTask->origin = burst;
	insert(head, tail, newTask);
}

void schedule(){
	int time = 0;
	int waiting = 0;
	int turnaround = 0;
	int response = 0;
	struct node *temp; 
	while (*head != NULL){
		temp = *head;
		while (temp != NULL){
			if (temp->task->burst == temp->task->origin){
				response += time;
			}

			if (temp->task->burst <= quantum){
				run(temp->task, temp->task->burst);
				time += temp->task->burst;
				temp->task->burst = 0;
			}
			else {
				run(temp->task, quantum);
				time += quantum;
				temp->task->burst -= quantum;
			}
			
			if (temp->task->burst == 0){
				waiting += time - temp->task->origin;
				turnaround += time;
				delete(head, tail, temp->task);
			}
			temp = temp->next;
		}
		
	}

	printf("\nAverage waiting time = %.2f\n", (float) waiting / count);
	printf("Average turnaround time = %.2f\n", (float) turnaround / count);
	printf("Average response time = %.2f\n", (float) response / count);
}

