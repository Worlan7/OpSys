/*
*
* xssh - a simple toy shell that executes certain described inbuilt functions.
* utility contains definitions for useful functions for the shell function
*
* Name : Elom Kwame Worlanyo
* Email : elomworlanyo@wustl.edu 
*
*/


/*
* Helper functions to drive behaviour of shell based on user input
*
*
*/
#ifndef UTIlITY_H
#define UTIlITY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h> 
#include <termcap.h>
#include <readline/readline.h>
#include <readline/history.h>


extern char **environ;      /* defined in libc */
/* Global variables */
int exit_status;  			/* exit status of shell (exit arg) */
int cmd_exit_status;		/* exit status of last foregound command ($?) */
int debug_level;			/* Debug level. Affects shell verbosity*/
int background;				/* Flag denoting background process. */
int pipe_flag;				/* Flag for pipeline*/
int redirect_in;			/* Flag denoting input redirection. */
int display_cmd;			/* Display command line after var substitution */
int defout, defin;			/* Default file descriptors */
int outfd, infd; 			/* Redirection file descriptors */

/* Current output stream. Defaults to stdout. Used in redirection. */
char* out_file_name;
char* in_file_name;

/* Child process PID array */
pid_t *child_pids, child_count; 
int child_cap;              
/* PID of last background command ($!) */
pid_t background_pid;   
/*PID of last foreground command */
pid_t fg_pid;

/*struct that stores local variables for the shell*/

typedef struct variable 
{
	char key[100];
	char value[100];
	int empty;
}variable;

/* Local var storage*/
variable * variables;

/* End global variables */


/* Function prototypes */
void init_env();
void usage(void);	/* Prints out program usage*/
char** eval(char* cmd, int* size);
int run_cmd(int argc, char *argv[]); 
int run_external_cmd(char *argv[]);
int run_pipe_cmd(int argc, char* argv[]);
int run_file_script(FILE *script);
char** var_substitute(char** argv, int argc);
void debug_message(char* message);

/*****************************
 * Builtin command routines
 *****************************/

int clr();
int echo(char* argv[]);
int print_environ();
int export_var(int argc, char* argv[]);
int unexport_var(int argc, char* argv[]);
int set_var(int argc, char* argv[]);
int unset_var(int argc, char* argv[]);
int list_directory(int argc, char* argv[]);
int change_directory(int argc, char* argv[]);
int show_history(int argc, char* argv[]);
int repeat_cmd(int argc, char* argv[]);
int kill_cmd(int sig, pid_t pid);
int pause_cmd();
int wait_cmd(pid_t pid);
int show_help(int argc, char* argv[]);


/*****************************
 * Signal Handling
 *****************************/
 
void sig_handler(int sig);

typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);


#endif

