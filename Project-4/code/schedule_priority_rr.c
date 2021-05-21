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
struct node** head_first_in_tail=NULL;
struct node** head_sort_by_priority=NULL;
void add(char *name, int priority, int burst)
{
    if(head_first_in_tail==NULL)
    {
        head_first_in_tail=malloc(sizeof(struct node* ));
        (*head_first_in_tail)=NULL;
    }

    //initialize a new task
    Task* newTask=(Task*) malloc(sizeof(Task));
    newTask->name=strdup(name);
    newTask->priority=priority;
    newTask->burst=burst;
    newTask->tid=__sync_fetch_and_add(&tid_value,1);

    insert(head_first_in_tail,newTask);
}

/*
The link is one where adding occurs in the head, so the first-coming task occurs in the tail of list.
*/
int single_priority_round_robin()
{
    struct node* temp=(*head_sort_by_priority);
    struct node* prev=temp;
    while(temp!=NULL)
    {
        if(temp->task->burst>QUANTUM)
        {
            if(temp->last_end_execution==0)
                total_response_time+=current_time;
            total_waiting_time+=(current_time - temp->last_end_execution);

            run(temp->task,QUANTUM);
            temp->task->burst-=QUANTUM;
            

            current_time+=QUANTUM;
            temp->last_end_execution=current_time;
        }
        else
        {
            if(temp->last_end_execution==0)
                total_response_time+=current_time;
            total_waiting_time+=(current_time-temp->last_end_execution);
            
            run(temp->task,temp->task->burst);
            // the task is over and should be deleted
            if(temp==(*head_sort_by_priority))//mean temp is the head
            {
                (*head_sort_by_priority)=temp->next;
            }
            else
            {
                prev->next=temp->next;
            }

            current_time+=(temp->task->burst);
            total_trunaround_time+=current_time;
            number_of_tasks++;
        }
        prev=temp;
        temp=temp->next;
    }
    if((*head_sort_by_priority)==NULL)
        return ALL_TASK_SCHEDULED;
    else
        return !ALL_TASK_SCHEDULED;
}
void schedule()
{
    if(head_sort_by_priority==NULL)
    {
        head_sort_by_priority=malloc(sizeof(struct node*));
        (*head_sort_by_priority)=NULL;
    }
    //turn around the list to make the first-comming list appear at the head of list
    while((*head_first_in_tail)!=NULL)
    {   
        struct node* temp=(*head_first_in_tail);
        struct node* prev=temp;
        struct node* min_priority=temp;
        struct node* prev_min_priority=min_priority;
        while(temp!=NULL)
        {
            if(temp->task->priority<min_priority->task->priority)
            {
                min_priority=temp;
                prev_min_priority=prev;
            }
            prev=temp;
            temp=temp->next;
        }
        insert(head_sort_by_priority,min_priority->task);//add the task with the smallest priority because the first inserted element will be in the tail
        //delete the node with max prority
        if(min_priority==(*head_first_in_tail))
        {
            (*head_first_in_tail)=min_priority->next;
        }
        else
        {
            prev_min_priority->next=min_priority->next;
        }
    }
    while(single_priority_round_robin()!=ALL_TASK_SCHEDULED);

    printf("Average turnaround time is %.3lf\n",total_trunaround_time/number_of_tasks);
    printf("Average response time is %.3lf\n",total_response_time/number_of_tasks);
    printf("Average waiting time is %.3lf\n",total_waiting_time/number_of_tasks);
}
