/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


#define MAX_LINE 80 /* 80 chars per line, per command */
#define EXIT "exit"
#define HISTORY "!!\n"
#define READ_END 0
#define WRITE_END 1


int main(void)
{
	char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
	for(int k = 0; k < MAX_LINE/2 + 1; k++) args[k] = NULL; //all init to NULL
    	
    	char input[MAX_LINE + 1]; //input[81]
    	
    	char history[MAX_LINE + 1]; //history[81]
    	int history_exist = 0; //for history
    	
    	int shell_input = dup(STDIN_FILENO); //for file input
    	int shell_output = dup(STDOUT_FILENO); //for file output
    	
    	int pipe_exist = 0; //for pipe
    	int pipe_pos;
    	int fd[2];
    	
    	int should_run = 1;
    	
    	int parent_wait; //for &
		
    	while (should_run)
    	{   
        	printf("osh>");
        	fflush(stdout);
        	parent_wait = 1;
        
        	//get input
        	fgets(input, MAX_LINE + 1, stdin);
        	
        	if(input[0] == '\n')
        	{
        		continue;
        	}
        	
        	//history command
        	if(strcmp(input,HISTORY) == 0)
        	{
        		if(history_exist == 0)
        		{
        			printf("No commands in history.\n");
        			continue;
        		}
        		else
        		{
        			if(strcmp(history,HISTORY) == 0)
        			{
        				printf("No commands in history.\n");
        				continue;
        			}
        			printf("%s\n", history);
        			strcpy(input, history);
        		}
        	}
        	
        	
        	//input is not !!
        	//process input
        	int args_ = 0;
        	args[args_] = (char*)malloc(sizeof(char)*20); //allocate space for each arg
        	int j = 0;
        	for(int i = 0; input[i] != '\n'; i++)
        	{
        		
        		if(input[i] != ' ' && input[i] != '\t' && input[i] != '\n')
        		{
        			args[args_][j] = input[i];
        			j++;
        			if(input[i] == '|') //there is a pipe
        			{
        				pipe_exist = 1;
        				pipe_pos = args_;
        			}
        		}
        		if(input[i] == ' ' || input[i] == '\t') 
        		{
        			if(input[i+1] != ' ' && input[i+1] != '\t' && input[i+1] != '\n')
        			{
					args[args_][j] = '\0';
					args_++;
					args[args_] = (char*)malloc(sizeof(char)*20); //allocate space for each arg
					j = 0;
        			}
        		}
        	}
        	args[args_][j] = '\0';
        	
        	//store history
        	strcpy(history, input);
        	history_exist = 1;
        	
        	//exit command
        	if(strcmp(args[0], EXIT) == 0) break;
        	
        	//if parent need to wait      	
        	if(strcmp(args[args_],"&") == 0)
 	 	{
 	 		args[args_] = NULL;
 	 		parent_wait = 0;
 	 	}
 	 	
 	 	//if there is a pipe
 	 	char *pipe_args[MAX_LINE/2+1];
 	 	for(int k = 0; k < MAX_LINE/2 + 1; k++) pipe_args[k] = NULL;
 	 	if(pipe_exist)
 	 	{	
 	 		for(int i = pipe_pos + 1, j = 0; i <=args_; i++, j++)
 	 		{
 	 			pipe_args[j] = (char*)malloc(sizeof(char)*20);
 	 			strcpy(pipe_args[j], args[i]); //put the command behind "|" into pipe_args
 	 			args[i] = NULL;
 	 			args_--;
 	 		}
 	 		args[pipe_pos] = NULL;
 	 	}
 	 	
 	 	
        	 
        	/**
         	 * After reading user input, the steps are:
         	 * (1) fork a child process
         	 * (2) the child process will invoke execvp()
         	 * (3) if command includes &, parent and child will run concurrently
         	 */
         	 pid_t pid;
         	 pid = fork();
         	 
         	 if(pid < 0) //error occurred
         	 {
         	 	fprintf(stderr, "Fork Failed");
         	 	return 1;
         	 }
         	 else if(pid == 0) //child process
         	 {
         	 	//redirect input and output
         	 	if(args_ >=1)
         	 	{
         	 		if(strcmp(args[args_-1], ">") == 0) //file output
         	 		{
         	 			int file_output = open(args[args_], O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
         	 			if(file_output == -1)
         	 			{
         	 				printf("Can't open the output file!");
         	 				continue;
         	 			}
         	 			args[args_-1] = NULL;
         	 			args[args_] = NULL;
         	 			dup2(file_output, STDOUT_FILENO); //redirect
         	 			execvp(args[0], args);
         	 			close(file_output);
         	 			dup2(shell_output, STDOUT_FILENO); //recover direct
         	 			exit(0);
         	 		}
         	 		else if(strcmp(args[args_-1], "<") == 0) //file input
         	 		{
         	 			int file_input = open(args[args_], O_RDONLY);
         	 			if(file_input == -1)
         	 			{
         	 				printf("Can't open the input file!");
         	 				continue;
         	 			}
         	 			args[args_-1] = NULL;
         	 			args[args_] = NULL;
         	 			dup2(file_input, STDIN_FILENO); //redirect
         	 			execvp(args[0], args);
         	 			close(file_input);
         	 			dup2(shell_input, STDIN_FILENO); //recover direct
         	 			exit(0);
         	 		}
         	 		
         	 		if(pipe_exist) //pipe communication
         	 		{
         	 			pid_t pid2;
         	 			if(pipe(fd) == -1) //create the pipe
         	 			{
         	 				fprintf(stderr, "Pipe failed");
         	 				return 1;
         	 			}
         	 			
         	 			pid2 = fork();
         	 			if(pid2 < 0) //error occurred
         	 			{
         	 				fprintf(stderr, "Fork Failed");
         	 				return 1;
         	 			}
         	 			else if(pid2 == 0) //child process
         	 			{
         	 				close(fd[READ_END]); //close the unused end of the pipe
         	 				dup2(fd[WRITE_END], STDOUT_FILENO); //output redirect to pipe's WRIRE_END
         	 				execvp(args[0], args); //first command output write into the pipe
         	 				close(fd[WRITE_END]);
         	 				dup2(shell_output, STDOUT_FILENO);
         	 				exit(0);
         	 			}
         	 			else // parent process
         	 			{
         	 				wait(NULL);
         	 				close(fd[WRITE_END]);
         	 				dup2(fd[READ_END], STDIN_FILENO); //input redirect to pipe's READ_END
         	 				execvp(pipe_args[0],pipe_args); //second command input read from pipe
         	 				close(fd[READ_END]);
         	 				dup2(shell_input, STDIN_FILENO);
         	 			}
         	 		}
         	 		else
         	 		{
         	 			execvp(args[0], args);
         	 		}
         	 	}
         	 	else
         	 	{
         	 		execvp(args[0], args);
         	 	}
         	 }
         	 else //parent process
         	 {
         	 	if(parent_wait)
         	 	{
         	 		wait(NULL);
         	 		printf("Child Complete\n");
         	 	}	
         	 }
         	 
         	 
         	 //clear
         	 for(int i = 0; i < MAX_LINE/2 + 1; i++) free(args[i]);
         	 
         	 
    	}
    
	return 0;
}
