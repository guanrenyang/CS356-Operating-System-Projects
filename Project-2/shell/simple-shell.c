/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

# include <stdio.h>
# include <fcntl.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/wait.h>
# include <sys/types.h>
#define MAX_LINE		80 /* 80 chars per line, per command */
#define REDIRECT_INPUT_FAIL 1
#define READ_END 0
#define WRITE_END 1

char **init_args(char* Instruction,char** args,char* last_parameter);
char **init_history(char** history_buffer,char** args,char* last_parameter);
void redirect_output(char** args);
int redirect_input(char** args,char * last_parameter);
int detect_pipe(char** args)
{
	for(int i=0;i<MAX_LINE/2+1;++i)
	{
		if(args[i]!=NULL && strcmp(args[i],"|")==0)// functions in string.h can't be passed a parameter which is NULL, so predetection is necessary
			return i;
	}
	return 0;
}
int main(void)
{
	char *args[MAX_LINE/2 + 1]={NULL};	/* command line (of 80) has max of 40 arguments */
	char *history_buffer[MAX_LINE/2 + 1]={NULL};
	int should_run = 1;
		
	while (should_run){   
		for(int i=0;i<MAX_LINE/2+1;++i)
			args[i]=NULL;

		printf("osh>");
		fflush(stdout);
		
	/*
	Read instructions and preprocess
	*/
		char * Instruction;
		char * last_parameter;
		Instruction=(char*) malloc(sizeof(char)*MAX_LINE);
		last_parameter=(char *) malloc (sizeof(char)*MAX_LINE);

		fgets(Instruction,MAX_LINE,stdin);
		init_args(Instruction,args,last_parameter);//store the seperate blocks into args[i], && is eliminated and stored in last_parameter, redirection is not done
		init_history(history_buffer,args,last_parameter);//reset the history_buffer according to args
		free(Instruction);
		free(last_parameter);
	/*
	args already set.
	files are rediected to STDIN/OUT
	*/

	//!!
		if(args[0]!=NULL&&strcmp(args[0],"!!")==0)
		{
			free(args[0]);
			args[0]=NULL;

			if(history_buffer[0]==NULL)
			{
				printf("No commands in history.\n");
				continue;
			}
			else
			{
				for(int i=0;i<MAX_LINE/2+1;++i)
				{
					
					if(history_buffer[i]==NULL)
						break;
					else
					{
						args[i]=(char*)malloc(MAX_LINE*sizeof(char));
						strcpy(args[i],history_buffer[i]);
					}
						
					printf("%s ",history_buffer[i]);
				}
				printf("\n");
			}
		}

	//exit
		if(args[0]!=NULL&&strcmp(args[0],"exit")==0)
		{
			should_run=0;
			continue;
		}
	//pipe
		int pipe_position=detect_pipe(args);
		if(pipe_position!=0)// pipe exists
		{
			pid_t pid;

			pid=fork();

			if(pid==0)//child process
			{
				int fd[2];
				pid_t pid;
				
				/*create a pipe*/
				if(pipe(fd)==-1)
				{
					fprintf(stderr,"Pipe failed");
					return 1;
				}

				pid=fork();

				if(pid>0)//parent process(actually child process of the initial one)
				{

					for(int i=pipe_position;i<MAX_LINE/2+1;++i)
					{
						free(args[i]);
						args[i]=NULL;
					}
					close(fd[READ_END]);

					dup2(fd[WRITE_END],STDOUT_FILENO);

					execvp(args[0],args);
					
				}
				else if(pid==0)//grandson process
				{
					strcpy(args[0],args[pipe_position+1]);
					for(int i=1;i<MAX_LINE/2+1;++i)
					{
						free(args[i]);
						args[i]=NULL;
					}
					close(fd[WRITE_END]);

					dup2(fd[READ_END],STDIN_FILENO);

					execvp(args[0],args);
				}
			}
			else // father process
			{
				wait(NULL);
			}
		}
		else
		{
			pid_t pid;
			pid=fork();

			if (pid==0) //child process
			{
				/*
				redirection output >
				*/
				redirect_output(args);
				redirect_input(args,last_parameter);//problem: if && in in.txt, it will not be operated	
				execvp(args[0],args);
				should_run=0;
			}
			else //parent process
			{
				if(strcmp(last_parameter,"&&")!=0)
					wait(NULL);
			}
		}
	}
 		
	
	for(int i=0;i<MAX_LINE/2+1;i++)
		free(args[i]);
	for(int i=0;i<MAX_LINE/2+1;i++)
		free(history_buffer[i]);
	
	return 0;
}
char **init_args(char* Instruction,char** args,char * last_parameter)
{
	//DeInstruction
	int Index_Instruction=0;
	int Index_args=0;
	char tmp=Instruction[Index_Instruction];
	int precious_is_space=0;
	
	while(tmp!='\n'&&tmp!=EOF)
	{
		if(Index_Instruction==0)
		{
			free(args[0]);
			args[0]=(char *) malloc(sizeof(char)*MAX_LINE);
			memset(args[0], 0, sizeof(args[0]));// after malloc, maybe some strings in args[0] is the input args=args+i, i do not now why

		}
		if(tmp==' ')
		{
			if(precious_is_space==0)
			{
				Index_args++;
				args[Index_args]=(char *)malloc(sizeof(char)*MAX_LINE);
				precious_is_space=1;
			}
		}
		else
		{
			strncat(args[Index_args],&tmp,1);
			
			precious_is_space=0;
		}
		
		Index_Instruction++;
		tmp=Instruction[Index_Instruction];	
	}

	//if no operands, args will be all NULL
	if(args[Index_args]!=NULL)
		strcpy(last_parameter,args[Index_args]);
	if(strcmp(last_parameter,"&&")==0)
		args[Index_args]=NULL;
}
char **init_history(char** history_buffer,char** args,char* last_parameter)
{	
	if(args[0]==NULL || strcmp(args[0],"!!")==0)
		return NULL;
	int i=0;
	for(i=0;i<MAX_LINE/2+1;i++)
	{
		if(args[i]==NULL)
			break;
		history_buffer[i]=(char*) malloc(MAX_LINE*sizeof(char));
		strcpy(history_buffer[i],args[i]);
	}
	if(strcmp(last_parameter,"&&")==0)
	{
		history_buffer[i]=(char*) malloc(MAX_LINE*sizeof(char));
		history_buffer[i]="&&";
	}
	
}
void redirect_output(char** args)
{
	for(int i=0;i<MAX_LINE/2+1;++i)
	{
		if(args[i]!=NULL && strcmp(args[i],">")==0)// > exists
		{
			char file_name[MAX_LINE];
			strcpy(file_name,args[i+1]);
			
			int fd=open(file_name,O_RDWR | O_NOCTTY | O_NDELAY);
			dup2(fd,STDOUT_FILENO);

			free(args[i]);
			free(args[i+1]);
			args[i]=NULL;
			args[i+1]=NULL;

			close(fd);
		}
	}
}
int redirect_input(char** args,char* last_parameter)
{
	for(int i=0;i<MAX_LINE/2+1;++i)
	{
		if(args[i]!=NULL && strcmp(args[i],"<")==0)// < exists
		{
			char file_name[MAX_LINE];
			strcpy(file_name,args[i+1]);
			
			int fd=open(file_name,O_RDWR | O_NOCTTY | O_NDELAY);
			
			free(args[i]);
			free(args[i+1]);
			args[i]=NULL;
			args[i+1]=NULL;
			dup2(fd,STDIN_FILENO);
			char Instruction[MAX_LINE*sizeof(char)];
			fgets(Instruction,MAX_LINE,stdin);
			init_args(Instruction,args+i,last_parameter);

			close(fd);
		}
	}
	return 0;
}