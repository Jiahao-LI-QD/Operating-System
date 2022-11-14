#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

struct node **head;
struct node **tail;
int count = 0;

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

	while (*head != NULL){
		run((*head)->task, (*head)->task->burst);
		response += time;
		time += (*head)->task->burst;
		waiting += time - (*head)->task->burst;
		turnaround += time;
		delete(head, tail, (*head)->task);
	}

	printf("\nAverage waiting time = %.2f\n", (float) waiting / count);
	printf("Average turnaround time = %.2f\n", (float) turnaround / count);
	printf("Average response time = %.2f\n", (float) response / count);
}

