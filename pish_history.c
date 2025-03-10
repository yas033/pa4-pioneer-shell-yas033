#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "pish.h"

char pish_history_path[1024] = {'\0'};

/*
 * Set history file path to ~/.pish_history.
 */
void set_history_path(){
    const char *home = getpwuid(getuid())->pw_dir;
    strncpy(pish_history_path, home, 1024);
    strcat(pish_history_path, "/.pish_history");
}

void add_history(const struct pish_arg *arg)
{
    // set history path if needed
    if (!(*pish_history_path)) {
        set_history_path();
    }

    /* 
     * TODO:
     * - open (and create if needed) history file at pish_history_path.
     * - write out the command stored in `arg`; argv values are separated
     *   by a space.
     */
     // open (and create if needed) history file at pish_history_path in append mode
     FILE *file = fopen(pish_history_path,"a");
     if(file==NULL){
         perror("open");
	 return;
     }
     //traverse the argv, write out the command stored in `arg`; argv values are separated by a space.
     for(int i=0; i < arg->argc; i++){
         if(i>0){
	     fprintf(file," ");
	 }
	 fprintf(file, "%s ",arg->argv[i]);
     }
     // \n afeter each command
     fprintf(file, "\n");

     //close file
     fclose(file);
}

void print_history()
{
    // set history path if needed
    if (!(*pish_history_path)) {
        set_history_path();
    }

    /* TODO: read history file and print with index */
    FILE *file = fopen(pish_history_path,"r");
    if(file==NULL){
        perror("open");
	return;
    }
    char line[1024];
    int count = 1; //start from line one
		
    //fgets each line and print out
    while(fgets(line,sizeof(line),file)){
        printf("%d %s", count++, line);
    }

    //close file
    fclose(file);
}  
