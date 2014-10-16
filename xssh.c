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

/* Bits in third argument to 'waitpid'. */
#define WNOHANG     1   /* Don't block waiting. */
#define WUNTRACED   2   /* Report status of stopped children. */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "xssh>> ";    /* command line prompt */
int verbose = 0;            /* if true, print additional output */
char sbuf[MAXLINE];         /* for composing sprintf messages */