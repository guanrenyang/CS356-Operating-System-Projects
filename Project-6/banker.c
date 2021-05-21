#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define SUCCESS 0
/*the available amount of each resource*/
int available[NUMBER_OF_RESOURCES]; //available[NUMBER_OF_RESOURCES]

/*the maximum demand of each cuotomer*/
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/*the amount currently allocated to each customer*/
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/*the remaining need of each customer*/
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int requset_resources(int customer_num,int requset[]);
void release_resources(int customer_num,int release[]);

void display();
//tool functions
int is_available(int requset[]);
int is_need(int customer_num,int request[]);
int satisfy_one_task(int Work[],int Finish[]);
int main(int argc,char* argv[])
{
        if(argc!=5)
        {
            fprintf(stderr,"ERROR: You must input the maximum number of each resource type");
            exit(1);
        }
    available[0]=atoi(argv[1]);
    available[1]=atoi(argv[2]);
    available[2]=atoi(argv[3]);
    available[3]=atoi(argv[4]);
    FILE* in;
    in=fopen("maximum.txt","r");
        if(in==NULL)
        {
            fprintf(stderr,"ERROR: You mut check the input file (Caution: input file name must be \"maximum.txt\"");
            exit(1);
        }

    int t=0;
    char* maximum_i=(char*) malloc(10*sizeof(char));
    while(fgets(maximum_i,10,in)!=NULL)
    {
        if(maximum_i[0]>'9'||maximum_i[0]<'1')
            break;
        char * temp=strdup(maximum_i);
        for(int i=0;i<NUMBER_OF_RESOURCES;++i)
        {
            maximum[t][i]=atoi(strsep(&temp,","));
            need[t][i]=maximum[t][i];
        }
        t++;
    }
    free(maximum_i);
    char* user_command=(char*)malloc(20*sizeof(char));
    while(fgets(user_command,20,stdin))
    {
        if(user_command[0]=='*')// * command, show all the information
        {
            display();
            continue;
        }
        char * temp=strdup(user_command);
        
        int temp_resources[NUMBER_OF_RESOURCES];

        char * command_name=strsep(&temp," ");
        int customer_num=atoi(strsep(&temp," "));
        for(int i=0;i<NUMBER_OF_RESOURCES;++i)
        {
            char * para=strsep(&temp," ");
            if(para==NULL)
            {
                fprintf(stderr,"Synax Error: Missing parameters\n");
                exit(1);
            }

            temp_resources[i]=atoi(para);
        }
        if(strcmp(command_name,"RQ")==0)//RQ command
        {
            int succ=requset_resources(customer_num,temp_resources);

            if(succ==SUCCESS)
                fprintf(stdout,"Satisfied\n");
            else
                fprintf(stdout,"Denied\n");
        }
        else if(strcmp(command_name,"RL")==0)//RL command
            release_resources(customer_num,temp_resources);
        else
        {
            fprintf(stderr,"Synax Error: Wrong command name\n");
            continue;
        }
    }
    return 0;
}
int requset_resources(int customer_num,int request[]){
    if(is_available(request)==!SUCCESS)
        return -1;
    if(is_need(customer_num,request)==!SUCCESS)
        return -1;
    for(int i=0;i<NUMBER_OF_RESOURCES;++i)
    {
        available[i]-=request[i];
        allocation[customer_num][i]+=request[i];
        need[customer_num][i]-=request[i];
    }
    //start banker's algorithm
    int Work[NUMBER_OF_RESOURCES];
    for(int i=0;i<NUMBER_OF_RESOURCES;++i)
        Work[i]=available[i];
    
    int Finish[NUMBER_OF_CUSTOMERS];
    for(int i=0;i<NUMBER_OF_CUSTOMERS;++i)
        Finish[i]=0;
    
    for(int i=0;i<NUMBER_OF_CUSTOMERS;++i)
    {
        int next_satisfied=satisfy_one_task(Work,Finish);

        if(next_satisfied==-1)
            return -1;
        
        Finish[next_satisfied]=1;

        for(int j=0;j<NUMBER_OF_RESOURCES;++j)//work=work+allocation
            Work[j]+=allocation[next_satisfied][j];
    }
    
    return SUCCESS;    
}
int is_available(int request[]){
    for(int i=0;i<NUMBER_OF_RESOURCES;++i)
    {
        if(request[i]>available[i])
            return -1;
    }
    return SUCCESS;
}
int is_need(int customer_num,int request[]){
    for(int i=0;i<NUMBER_OF_RESOURCES;++i)
    {
        if(request[i]>need[customer_num][i])
            return -1;
    }
    return SUCCESS;
}
int satisfy_one_task(int Work[],int Finish[]){
    for(int i=0;i<NUMBER_OF_CUSTOMERS;++i)
    {
        if(Finish[i]!=0)
            continue;
        int flag=0;
        for(int j=0;j<NUMBER_OF_RESOURCES;++j)
        {
            if(need[i][j]>Work[j])
            {
                flag=1;
                break;
            }
        }
        if(flag==0)
            return i;
    }
    return -1;
}
void release_resources(int customer_num,int release[]){
    for(int i=0;i<NUMBER_OF_RESOURCES;++i)
    {
        if(allocation[customer_num][i]<release[i])
        {
            fprintf(stdout,"Customer %d has only %d instances of resource type %d,\n but you want to release %d instances.\nAll instances are released.\n",customer_num,allocation[customer_num][i],i,release[i]);
            available[i]+=allocation[customer_num][i];
            allocation[customer_num][i]=0;    
        }
        else
        {
            available[i]+=release[i];
            allocation[customer_num][i]-=release[i];
        }
    }
}
void display(){
    //display available
    fprintf(stdout,"Available:\n");
    for(int i=0;i<NUMBER_OF_RESOURCES;++i)    
        fprintf(stdout,"%d, ",available[i]);        
    fprintf(stdout,"\n");

    //display Maximum
    fprintf(stdout,"Maximum:\n");
    for(int i=0;i<NUMBER_OF_CUSTOMERS;++i)
    {
        for(int j=0;j<NUMBER_OF_RESOURCES;++j)
        {
            fprintf(stdout,"%d, ",maximum[i][j]);
        }
        fprintf(stdout,"\n\n");
    }

    //display Allocation
    fprintf(stdout,"Allocation:\n");
    for(int i=0;i<NUMBER_OF_CUSTOMERS;++i)
    {
        for(int j=0;j<NUMBER_OF_RESOURCES;++j)
        {
            fprintf(stdout,"%d, ",allocation[i][j]);
        }
        fprintf(stdout,"\n\n");
    }
    //display need
    fprintf(stdout,"Need:\n");
    for(int i=0;i<NUMBER_OF_CUSTOMERS;++i)
    {
        for(int j=0;j<NUMBER_OF_RESOURCES;++j)
        {
            fprintf(stdout,"%d, ",need[i][j]);
        }
        fprintf(stdout,"\n\n");
    }
}
