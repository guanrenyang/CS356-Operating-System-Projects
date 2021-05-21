#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"        //define a function "run" which print the current running task 
#include "list.h"       //a linked list where adding and deleting occur in the head of it.
#include "schedulers.h" //define two functions "add" and "schedule", which should be completed in this file
#include "task.h"       //include struct Task

#define ALL_TASK_SCHEDULED 0
double total_waiting_time=0;
double total_response_time=0;
double total_trunaround_time=0;
double number_of_tasks;

int current_time=0;

unsigned int tid_value=0;
struct node** head_first_in_head=NULL;

void add(char *name, int priority, int burst)
{
    if(head_first_in_head==NULL)
    {
        head_first_in_head=malloc(sizeof(struct node* ));
        (*head_first_in_head)=NULL;
    }

    //initialize a new task
    Task* newTask=(Task*) malloc(sizeof(Task));
    newTask->name=strdup(name);
    newTask->priority=priority;
    newTask->burst=burst;
    newTask->tid=__sync_fetch_and_add(&tid_value,1);

    insert(head_first_in_head,newTask);
}

/*
The link is one where adding occurs in the head, so the first-coming task occurs in the tail of list.
*/
int schedule_single_task()
{
    struct node* temp=(*head_first_in_head);
    struct node* prev;
    while(temp->next!=NULL)
    {
        prev=temp;
        temp=temp->next;
    }

    //stimulation of running the task which is in the tail of list
    //counting associated information(waitingtime, turnaround time, response time)

    total_waiting_time+=current_time;
    total_response_time+=current_time;    

    run(temp->task,temp->task->burst);

    current_time+=temp->task->burst;
    total_trunaround_time+=current_time;
    number_of_tasks++;

    //after simulation, delete the node from the list
    prev->next=NULL;
    if(temp==(*head_first_in_head))
        return ALL_TASK_SCHEDULED;
    else
        return !ALL_TASK_SCHEDULED;
}
/*
there are two differences of adding/deleting elements in the list
in fcfs:
    There is no conditional branch in the while loop in line 46, so prev may be the last element.
    As a reuslt, the line 65 may be an segmentation fault.
    This is a special case because the deleted element is always the last element;
in sjf, priority:
    The conditional branch ensures that pre_of_minTime and minTime are not NULL,
    so the deleting operation makes sense.
*/
void schedule()
{
    while(schedule_single_task()!=ALL_TASK_SCHEDULED);
    printf("Average turnaround time is %.3lf\n",total_trunaround_time/number_of_tasks);
    printf("Average response time is %.3lf\n",total_response_time/number_of_tasks);
    printf("Average waiting time is %.3lf\n",total_waiting_time/number_of_tasks);
    
}