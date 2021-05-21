#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_INPUT 20

#define RQ 1
#define RL 2
#define C 3
#define STAT 4

#define Fail -1

short *memory;//0 empty, 1 full

int *memory_size;
struct processor
{
    char * name;
    int starting_address;
    int ending_address;
    struct processor * next;
};

struct processor *head;
// list function
void insert(char* name, int starting_address, int ending_address);
struct processor * search_by_address(int i);
struct processor * search_by_name(char * name);
struct processor * delete_by_name(char * name);

int Decode_Input(char * input);

int request(char * input);
int release(char * input);
void compact();
void status();

int first_fit(int size);
int worst_fit(int size);
int best_fit(int size);

int main(int argc,char * argv[])
{
    memory_size=(int*)malloc(sizeof(int));
    (*memory_size)=atoi(argv[1]);
    memory=(short*) malloc(sizeof(short)*(*memory_size));
    
    fprintf(stdout,"allocator>");

    char * input=malloc(MAX_INPUT*sizeof(char));
    while (fgets(input,MAX_INPUT-1,stdin))
    {

        int command=Decode_Input(input);
        switch (command)
        {
        case RQ:
            request(input);
            break;
        case RL:
            release(input);
            break;
        case C:
            compact();
            break;
        case STAT:
            status();
        default:
            break;
        }
        fprintf(stdout,"allocator>");
    }
    
}
int Decode_Input(char * input)
{
    if(input[0]=='C')
        return C;
    else if(input[0]=='S')
        return STAT;
    else if(input[1]=='Q')
        return RQ;
    else if(input[1]=='L')
        return RL;    
}
int request(char *input)
{
    char * command=strsep(&input," ");
    char * name=strsep(&input," ");
    int size=atoi(strsep(&input," "));
    char * strategy=strsep(&input," ");
    //ok

    int start;
    if(strategy[0]=='F')
        start=first_fit(size);
    else if(strategy[0]=='W')
        start=worst_fit(size);
    else if(strategy[0]=='B')
        start=best_fit(size);

    for(int i=0;i<size;++i)
    {
        memory[start+i]=1;
    }
    insert(name,start,start+size-1);
    
}
int release(char * input)
{
    char * command=strsep(&input," ");
    char * name=strsep(&input,"\n");

    struct processor * temp=delete_by_name(name);
    for(int i=temp->starting_address;i<=temp->ending_address;++i)
        memory[i]=0;
    free(temp);
}

void compact()
{
    struct processor *new_head=NULL;
    for(int i=0;i<(*memory_size);++i)
    {
        while (head!=NULL)
        {
            int size=head->ending_address-head->starting_address+1;

            for(int j=0;j<size;j++)
                memory[i+j]=1;

            head->starting_address=i;
            head->ending_address=i+size-1;
            struct processor * temp=head;
            head=head->next;

            // all temp to the new head
            if(new_head==NULL)
            {
                new_head=temp;
                new_head->next=NULL;
            }
            else
            {   
                temp->next=new_head;
                new_head=temp;
            }
            i=i+size;
        }
        memory[i]=0;     
    }
    head=new_head;    
}
void status()
{
    
    int start=0;
    int end=start;
    int prev=0;
    for(int i=0;i<(*memory_size);++i)
    {
        if(prev==0&&memory[i]==1&&i!=0)
        {
            fprintf(stdout,"Addresses [%d:%d] Unuesd\n",start,end);
        }
        else if(prev==1&&memory[i]==0)
        {
            start=end=i;
            prev=0;
            if(i==(*memory_size)-1)
                fprintf(stdout,"Addresses [%d:%d] Unuesd\n",start,end);
        }
        else if(prev==0&&memory[i]==0)
        {
            end=i;
            if(i==(*memory_size)-1)
                fprintf(stdout,"Addresses [%d:%d] Unuesd\n",start,end);
        }
        if(memory[i]==1)
        {
            struct processor* proc=search_by_address(i);
            fprintf(stdout,"Addresses [%d:%d] Process %s\n",proc->starting_address,proc->ending_address,proc->name);
            i=proc->ending_address;
            prev=1;
        }
    }
    
}

int first_fit(int size)
{
    int start=0;
    int end=start;
    int prev=1;
    int sum=0;

    for(int i=0;i<(*memory_size);i++)
    {
        if(prev==1&&memory[i]==0)
        {
            start=i;
            end=start;
            prev=0;
            sum++;
        }
        else if(prev==0&&memory[i]==0)
        {
            sum++;
            end=i;
        }
        else if(prev==0&&memory[i]==1)
        {
            prev=1;
            sum=0;
        }
        
        if(sum>=size)
        {
            return start;
        }
    }
    return -1;
}
int worst_fit(int size)
{
    int start=0;
    int end=start;
    int prev=1;
    int sum=0;

    int max=-1;
    int max_start=-1;
    for(int i=0;i<(*memory_size);i++)
    {
        if(prev==1&&memory[i]==0)//start of contious empty memory
        {
            start=i;
            prev=0;
            sum++;
        }
        else if(prev==0&&memory[i]==0&&i!=((*memory_size)-1))
        {
            sum++;
        }
        else if(prev==0&&memory[i]==1)//end of contious empty memory
        {
            prev=1;
            sum=0;
        }

        if((i==((*memory_size)-1)&&memory[i]==0)||(memory[i]==0&&memory[i+1]==1))
        {
            if(sum>max)
            {
                max=sum;
                max_start=start;
            }
        }
        
    }
    if(max_start!=-1)
        return max_start;
    else
        return -1;
}
int best_fit(int size)
{
    int start=0;
    int end=start;
    int prev=1;
    int sum=0;

    int min=(*memory_size)+1;
    int min_start=-1;
    for(int i=0;i<(*memory_size);i++)
    {
        if(prev==1&&memory[i]==0)//start of contious empty memory
        {
            start=i;
            prev=0;
            sum++;
        }
        else if(prev==0&&memory[i]==0&&i!=((*memory_size)-1))
        {
            sum++;
        }
        else if(prev==0&&memory[i]==1)//end of contious empty memory
        {
            prev=1;
            sum=0;
        }

        if((i==((*memory_size)-1)&&memory[i]==0)||(memory[i]==0&&memory[i+1]==1))//if till the end of one block
        {
            if(sum<min&&sum>=size)
            {
                min=sum;
                min_start=start;
            }
        }
    }
    if(min_start!=-1)
        return min_start;
    else
        return -1;
}

void insert(char* name, int starting_address, int ending_address)
{
    if(head==NULL)
    {
        head=(struct processor*) malloc(sizeof(struct processor));
        head->name=strdup(name);
        head->starting_address=starting_address;
        head->ending_address=ending_address;
        head->next=NULL;
    }
    else
    {
        struct processor * temp=(struct processor*) malloc(sizeof(struct processor));
        temp->name=strdup(name);
        temp->starting_address=starting_address;
        temp->ending_address=ending_address;
        
        temp->next=head->next;
        head->next=temp;
    }
}
struct processor * search_by_address(int i)
{
    if(head==NULL)
        return NULL;

    struct processor *temp=head;
    struct processor *prev;
    while (temp!=NULL)
    {
        if(temp->starting_address==i)
            return temp;
        prev=temp;
        temp=temp->next;
    }
    return NULL;
}
struct processor * search_by_name(char * name)
{
    if(head==NULL)
        return NULL;

    struct processor *temp=head;
    struct processor *prev;
    while (temp!=NULL)
    {
        if(strcmp(name,temp->name)==0)
            return temp;
        prev=temp;
        temp=temp->next;
    }
    return NULL;
}
struct processor * delete_by_name(char * name)
{
    if(head==NULL)
        return NULL;

    struct processor *temp=head;
    struct processor *prev;
    while (temp!=NULL)
    {
        if(strcmp(name,temp->name)==0)
        {
            if(temp==head)
                head=temp->next;
            else
                prev->next=temp->next;
            return temp;
        }
            
        prev=temp;
        temp=temp->next;
    }
    return NULL;
}
