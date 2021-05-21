/*
 * Driver.c
 *
 * Schedule is in the format
 *
 *  [name] [priority] [CPU burst]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"

#define SIZE    100

int main(int argc, char *argv[])
{
    FILE *in;
    char *temp;
    char task[SIZE];

    char *name;
    int priority;
    int burst;

    
    in = fopen(argv[1],"r");// the test file name is the first parameter of ./
    
    /*
    strsep separates the string by the delimiter.
    the former half is stored in the returning pointer
    the later half is stored in the parameter-pointer passing in 
    */
    
    while (fgets(task,SIZE,in) != NULL) {
    
        temp = strdup(task);
        name = strsep(&temp,",");
        priority = atoi(strsep(&temp,","));
        burst = atoi(strsep(&temp,","));

        // add the task to the scheduler's list of tasks
        add(name,priority,burst);
        
        free(temp);
    }
  
    fclose(in);

    // invoke the scheduler

    schedule();

    return 0;
}
