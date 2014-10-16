/*
*
* xssh - a simple toy shell that executes certain described inbuilt functions
*
* Name : Elom Kwame Worlanyo
* Email : elomworlanyo@wustl.edu 
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "xssh>> ";    /* command line prompt */
int verbose = 0;            /* if true, print additional output */
int disp_cmdl = 0;		/* if true, display command line after var sub */
char *file_args;	/* Any optional file arguments go here*/ 
char sbuf[MAXLINE];         /* for composing sprintf messages */
/* End global variables */


void usage(void);


/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "xd:f:")) != EOF) {
        switch (c) {
        case 'x':           /* print the command line after variable */ 
            printf("x is in \n");
            disp_cmdl = 1;		/* substitution is displayed before */
        break;		       	/* the command is evaluated. w/e that means*/
      
        case 'd':             /* emit additional diagnostic info */
            printf("%s", optarg);
            printf(" d is in \n");
        	if(isdigit(*optarg)){
            	verbose = atoi(optarg);
            }else{
            	usage();
            }
        break;
       
        case 'f':             /* change input from stdin to file */
          	file_args = optarg ;  /* optional args assigned to shell vars */
         	printf("%s", file_args);
           	printf(" f is in \n");
          	exit;
        break;
    default:
            usage();
    }
    }

    /* Execute the shell's read/eval loop */
    while (1) {

    } 

    exit(0); /* control never reaches here */
}



/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

