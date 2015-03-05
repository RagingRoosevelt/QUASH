#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define BSIZE 256

using namespace std;

/*
* [ ] Run executables without arguments (10)
* [ ] Run executables with arguments (10)
* [x] set for HOME and PATH work properly (5)
* [x] exit and quit work properly (5)
* [x] cd (with and without arguments) works properly (5)
* [ ] PATH works properly. Give error messages when the executable is not found (10)
* [ ] Child processes inherit the environment (5)
* [ ] Allow background/foreground execution (&) (5)
* [ ] Printing/reporting of background processes, (including the jobs command) (10)
* [ ] Allow file redirection (> and <) (5)
* [ ] Allow (1) pipe (|) (10)
* [ ] Supports reading commands from prompt and from file (10)
* [ ] Report (10)
*
* Bonus points:
* [ ] Support multiple pipes in one command (10)
* [ ] kill command delivers signals to background processes. The kill
* command has the format: kill SIGNUM, JOBID, where SIGNUM is an integer
* specifying the signal number, and JOBID is an integer that specifies
* the job that should receive the signal (5)
*/

struct command {
	char instr[BSIZE];
	char args[BSIZE][BSIZE];
	int arg_count;
};

#include "execute.c"
#include "cd.c"
#include "job.c"
//#include "tokenize.c"


int main(int argc, char **argv,char **envp)
{
	
	printf("Welcome to QUASH\n");
	printf("Developed by Theodore S Lindsey & Samuel A Lamb\n");
	printf("Home Directory:    %s\n", getenv("HOME"));
	printf("Current Directory: %s\n\n", getcwd(NULL, 0));
	printf("PATH:              %s\n\n", getenv("PATH"));
	
	char user_input[BSIZE];
	char temp[BSIZE];
	char filename[BSIZE];
	int fromfile = 0;
	FILE* file;
	int fw;
	
	while ((strcmp(user_input, "exit")!=0) && (strcmp(user_input, "quit")!=0))
	{	// display QUASH prompt and get user input.	
		printf(strcat(getcwd(NULL, 0), "> "));
		// Get input from user or from file
		/* if (fromfile)
		{
			if(fgets (user_input, sizeof(user_input), file))
			{ // Next line read from file
				// Strips \n from input
				size_t len = strlen(user_input);
				if (len > 0 && user_input[len-1] == '\n') {
					user_input[--len] = '\0';
				}
			}
			else 
			{ // EOF reached, switch back to user input
				fromfile = 0;
				fclose(file);
				fgets(user_input, sizeof(user_input), stdin);
			}
		}
		else
		{ // Get user input from stdin.
			fgets(user_input, sizeof(user_input), stdin);
		} */
		
		
		//if (fromfile && !(fgets(user_input, sizeof(user_input), stdin)))
		//{
			//fclose(file);
		//	fromfile = 0;
			fgets(user_input, sizeof(user_input), stdin);
		//}
		
		struct command current_cmd;
		
		/*******************************
		 * tokenize user input into command and arguments 
		 *******************************/
		char *token;
		// Clear out contents of the current_args array and current_cmd
		strcpy(current_cmd.instr, "");
		for (int i=0; i<BSIZE; i++)
		{ strcpy(current_cmd.args[i], ""); }

		// Initialize tokenization 
		token = strtok(user_input, " \n");
		// Assume that first token is the command rather than arguments
		strncpy(current_cmd.instr, token, sizeof(current_cmd.instr)-1);
		printf( "command: %s\n", current_cmd.instr );
		// Advance token location
		token = strtok(NULL, " \n");
		/* walk through other tokens */
		current_cmd.arg_count = 0;
		while( token != NULL ) 
		{	// Extract current token to next element of the argument array
			strncpy(current_cmd.args[current_cmd.arg_count], token, sizeof(current_cmd.args[current_cmd.arg_count])-1);
			// Advance token location
			token = strtok(NULL, " \n");
			// Increment count of arguments
			current_cmd.arg_count++;
		}
		
		
		// Print arguments gathered
		for (int i=0; i<current_cmd.arg_count; i++)
		{ printf("Arguments: '%s'\n", current_cmd.args[i]); }
		
		
		
		/*******************************
		 * Built-in command processing 
		 *******************************/
		// pwd
		if (strcmp(current_cmd.instr, "pwd") == 0)
		{
			if (getcwd(temp, sizeof(temp)) == NULL)
			{
				perror("getcwd() error");
			}
		
			printf(strcat(temp, "\n"));
		// cd
		} else if (strcmp(current_cmd.instr, "cd") == 0) {
			cd(current_cmd.args[0]);
		} 
		// set
		else if (strcmp(current_cmd.instr, "set") == 0) 
		{
			// set PATH
			if (strcmp(current_cmd.args[0],"PATH") == 0)
			{
				setenv ("PATH",strcat(strcat(getenv("PATH"),":"),current_cmd.args[1]),1);
				printf("PATH:              %s\n\n", getenv("PATH"));
			}
			// set HOME
			else if (strcmp(current_cmd.args[0],"HOME") == 0)
			{
				setenv("HOME",current_cmd.args[1],1);
				printf("Home Directory:    %s\n", getenv("HOME"));
			}
			else{
				printf("Please specify PATH or HOME.");
			}
		}
		// jobs
		// Theoretically, anyway.
		else if (strcmp(current_cmd.instr,"jobs") == 0)
		{
			for (int i = 0; i < numjobs; i++)
			{
				printf("[%i] %i %s\n", joblist[i].jobID, joblist[i].jobpid, joblist[i].jobcommand);
			}
		}
		else if (strcmp(current_cmd.instr,"<") == 0)
		{
			file = freopen(current_cmd.args[0], "r", stdin);
			//fw = fopen(current_cmd.args[0], "r");
			//dup2(fw,STDIN_FILENO);
			fromfile = 1;
		}
		else {
		/*******************************
		 * External command processing 
		 *******************************/
			if (execute(current_cmd.instr, current_cmd.args, current_cmd.arg_count)==0)
			{ printf("error: command not found\n"); }				
		}
		
	}

	return 0;
}