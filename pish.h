#ifndef __PISH_H__
#define __PISH_H__

#define MAX_ARGC 64
/*
 * Each input command will be parsed into this struct.
 * E.g. 
 *   If input is "du -h -d 1", we will have
 *      argc    = 4
 *      argv[0] = "du"
 *      argv[1] = "-h"
 *      argv[2] = "-d"
 *      argv[3] = "1"
 *      argv[4] = NULL
 *   It is IMPORTANT that the array is terminated by a NULL element.
 * This is different from the strings being NULL-terminated. If there
 * are 4 elements as in the example above, then argv[0] thru argv[3]
 * should contain the individual arguments, but argv[4] should be NULL.
 * This makes it easy to use argv to call `execvp()`.
 */
struct pish_arg {
    int argc;               /* The number of arguments */
    char *argv[MAX_ARGC];   /* NULL-terminated array of argument strings */
};

void handle_exit(struct pish_arg *arg);
void handle_cd(struct pish_arg *arg);
void handle_history(struct pish_arg *arg);
void execute_external_command(struct pish_arg *arg);

void add_history(const struct pish_arg *arg);

void print_history();

void usage_error(void);

//declare`
extern char pish_history_path[1024];
void set_history_path();
#endif // __PISH_H__
