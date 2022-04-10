#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "schedulers.h"
#include "task.h"
#include "list.h"

struct node *head = NULL;
int tid_value = 0;

void add(char *name, int priority, int burst)
{
	Task *tsk;
	tsk = (Task*) malloc(sizeof(Task));
	tsk -> tid = __sync_fetch_and_add(&tid_value, 1);
	tsk -> name = name;
	tsk -> priority = priority;
	tsk -> burst = burst;
	insert(&head, tsk);
}

void schedule()
{
	while(1)
	{
		if(head == NULL)
		{
			break;
		}
		
		//for a time unit, fcfs
		struct node *cur_node = head;
		while(cur_node -> next != NULL)
		{
			cur_node = cur_node -> next; 
		}
		
		Task *cur_task = cur_node -> task;
		
		if(cur_task -> burst <= QUANTUM) //can be finished in a time unit
		{
			run(cur_task, cur_task-> burst);
			delete(&head, cur_task);
		}
		else //can't be finished in a time unit, add it to list as a final coming task
		{
			run(cur_task, QUANTUM);
			delete(&head, cur_task);
			
			cur_task -> burst = cur_task -> burst - QUANTUM;
			insert(&head, cur_task);
		}
	}
}
