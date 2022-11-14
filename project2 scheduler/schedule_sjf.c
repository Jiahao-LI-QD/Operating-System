#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

#define MAX_INT ((unsigned)(-1)>>1)

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
	int s;
	struct node *shortest;
	struct node *temp;

	while (*head != NULL){
		temp = *head;
		s = MAX_INT;
		while (temp != NULL){
			if (temp->task->burst < s){
				s = temp->task->burst;
				shortest = temp;
			}
			temp = temp->next;
		}
		run(shortest->task, shortest->task->burst);
		response += time;
		waiting += time;
		time += shortest->task->burst;
		turnaround += time;
		delete(head, tail, shortest->task);
	}

	printf("\nAverage waiting time = %.2f\n", (float) waiting / count);
	printf("Average turnaround time = %.2f\n", (float) turnaround / count);
	printf("Average response time = %.2f\n", (float) response / count);
}
