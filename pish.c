#include <ctype.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "pish.h"

//Batch mode flag. If set to 0, the shell reads from stdin. If set to 1,
//     * the shell reads from a file from argv[1].
static int script_mode = 0;

//if not in batch mode, return 1
int prompt(void){
    static const char prompt[] = {0xe2, 0x96, 0xb6, ' ', ' ', '\0'};
    if (!script_mode) {
	printf("%s", prompt);
        fflush(stdout);
    } return 1;
}

//handles the exit command, exit the shell if called with no argument
//if extra arguments, prints a usage error
void handle_exit(struct pish_arg *arg){
    if(arg->argc >1 ){
        usage_error();
	return;
    }
     exit(EXIT_SUCCESS);  //exit the shell 
}

//handles the cd command
//change the curr working directory to the target path
//chech the number of arg if is incorrect prints isage error
//if the change directory fails prints error
void handle_cd(struct pish_arg *arg){
     if(arg->argc != 2){
         usage_error();
	 //fprintf(stderr,"pish: Usage error\n");
	 return;
     }
     if(chdir(arg->argv[1])!=0){
         perror("cd");
     }
}

//handles the history command
//"r"and prints  command history from the file
//if called with arg, print error
void handle_history(struct pish_arg *arg){
     if(arg->argc != 1){
         usage_error();
	 return;
     }
     FILE *file = fopen(pish_history_path, "r");
     if(!file){          //check if file opening failed
         perror("open");
	 return;
     }
     char line[1024];
     int count =1;
     while(fgets(line,sizeof(line),file)){
          printf("%d%s", count++ , line);
     }
     fclose(file); //close file after "r"
}

//executes an external command by fork a child process
//the child process replaces itself with the requested command using execvp()
//if the fork or exec fails, print error
void execute_external_command(struct pish_arg *arg){
     pid_t pid = fork();
     if(pid < 0){        //fork failed
         perror("fork");
	 return;
     }
     else if( pid == 0){  //child process
         execvp(arg->argv[0],arg->argv);   //replace the child process with the command
	 perror("pish");                  //print error if execvp fails
	 exit(EXIT_FAILURE);              //exit child process with FAILURE
     }
     else{       //parent process
     wait(NULL); //wait until the child finish the tasks
     }
}

/*
 * Prints a prompt IF NOT in batch mode (see script_mode global flag),
 */ 
/*int prompt(void)
{
    static const char prompt[] = {0xe2, 0x96, 0xb6, ' ', ' ', '\0'};
    if (!script_mode) {
        printf("%s", prompt);
        fflush(stdout);
    }
    return 1;
}*/

/*
 * Print usage error for built-in commands.
 */
void usage_error(void)
{
    fprintf(stderr, "pish: Usage error\n");
    fflush(stdout);
}

/*
 * Break down a line of input by whitespace, and put the results into
 * a struct pish_arg to be used by other functions.
 *
 * @param command   A char buffer containing the input command
 * @param arg       Broken down args will be stored here.
 */
void parse_command(char *command, struct pish_arg *arg)
{
    // TODO
    // 1. Clear out the arg struct
    arg->argc = 0;
    // 2. Parse the `command` buffer and update arg->argc & arg->argv.
    char *token = strtok(command, " \t\n"); //find token by space, \t \n
    while (token != NULL && arg->argc < MAX_ARGC -1){
    	arg->argv[arg->argc++] = token; //store token in argv, and argc++
    	token = strtok(NULL," \t\n"); //find next token
    }
    arg->argv[arg->argc]=NULL; //Null-terminate argv for execvp()
}

/*
 * Run a command.
 *
 * Built-in commands are handled internally by the pish program.
 * There are three built-in commands to implement for this project: exit, cd, and history
 * Otherwise, use fork/exec to create child process to run the program.
 *
 * If the command is empty, do nothing.
 * If NOT in batch mode, add the command to history file.
 */
void run(struct pish_arg *arg)
{
    // TODO
    // if the command is empty, do nothing.
    if(arg->argv==0){
        return;
    }
    //* If NOT in batch mode, add the command to history file.
    if(!script_mode){
        add_history(arg);
    }
    //exit, cd and history
    if(strcmp(arg->argv[0],"exit")==0){
        handle_exit(arg);
	return;
    }
    if(strcmp(arg->argv[0],"cd")==0){
        handle_cd(arg);
        return;	
    }
    if(strcmp(arg->argv[0],"history")==0){
	handle_history(arg);
        return;
    }

    execute_external_command(arg);
}

/*
 * The main loop. Continuously reads input from a FILE pointer
 * (can be stdin or an actual file) until `exit` or EOF(end-of-file-marker).
 */
int pish(FILE *fp)
{
    // assume input does not exceed buffer size
    char buf[1024];
    struct pish_arg arg;

    /* TODO:
     * Figure out how to read input and run commands on a loop.
     * It should look something like:

            while (prompt() && [TODO] {
                parse_command(buf, &arg);
                run(&arg);
            }

     * The [TODO] is where you read a line from `fp` into `buf`.
     */
    while(1){
	if(fp==stdin){
	prompt();
	}

	if( fgets(buf, sizeof(buf),fp)==NULL){
		break;
	}
        parse_command(buf, &arg);
        run(&arg);
    }

    if(fp!=stdin){
    fclose(fp);
    
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    FILE *fp;

    /* TODO: 
     * Set up fp to either stdin or an open file.
     * - If the shell is run without argument (argc == 1), use stdin.
     * - If the shell is run with 1 argument (argc == 2), use that argument
     *   as the file path to read from.
     * - If the shell is run with 2+ arguments, call usage_error() and exit.
     */
    if(argc == 1){
        fp = stdin;
    }else if(argc == 2){
        fp = fopen(argv[1],"r");
	//open: No such file or directory
	if(fp==NULL){
	    perror("open");
	    return EXIT_FAILURE;
	}
    }else{
        usage_error();
	return EXIT_FAILURE;
    }

    //run the shell
    pish(fp);

    /* TODO:
     * close fp if fp is not stdin.
     */  
    if(fp!=stdin){
        fclose(fp);
	return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}
