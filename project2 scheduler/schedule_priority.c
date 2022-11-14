#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

struct node *head[10];
struct node *tail[10];
int count = 0;

void add(char *name, int priority, int burst){
	struct task *newTask = malloc(sizeof(struct task));
	if (head == NULL){
		int i = 0;
		while (i < 10){
			head[i] = malloc(sizeof(head));
			tail[i] = malloc(sizeof(head));
		}
	}

	newTask->name = name;
	newTask->tid = count++;
	newTask->priority = priority;
	newTask->burst = burst;
	newTask->origin = burst;

	insert(&head[priority-1], &tail[priority-1], newTask);
}

void schedule(){
	int time = 0;
	int waiting = 0;
	int turnaround = 0;
	int response = 0;

	int i = 9;
	while ( i >= 0 ){
		while (head[i] != NULL){
			run(head[i]->task, head[i]->task->burst);
			response += time;
			waiting += time;
			time += head[i]->task->burst;
			turnaround += time;
			delete(&head[i], &tail[i], head[i]->task);
		}
		i--;
	}

	printf("\nAverage waiting time = %.2f\n", (float) waiting / count);
	printf("Average turnaround time = %.2f\n", (float) turnaround / count);
	printf("Average response time = %.2f\n", (float) response / count);
}