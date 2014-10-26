/*
*
* xssh - a simple toy shell that executes certain described inbuilt functions.
* utility contains definitions for useful functions for the shell function
*
* Name : Elom Kwame Worlanyo
* Email : elomworlanyo@wustl.edu 
*
*/

#include "utility.h"

/***********************
 * helper routines
 ***********************/

/*
* init_env - Function to initialize the shell environment for use
*/
void init_env()
{
    /*
     *  $$: the PID of this xssh instance 
     *  $?: the exit status of the last completed foreground command
     *  $!: the PID of the last dispatched background command   
     */
    
    /* Initialising local variables*/
    int i;
    variables = malloc(sizeof(variable)*100);
	for (i = 0; i < 100; ++i) {
		variables[i].empty = 1;
	}

	/* Install the signal handlers */
    Signal(SIGINT,  sig_handler);   /* ctrl-c */
    Signal(SIGQUIT, sig_handler);
    Signal(SIGTSTP, sig_handler);  /* ctrl-\ */ 
    Signal(SIGCONT, sig_handler);
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child*/ 

	/* Storing shell variables*/
    char instance_pid[50], last_exit_status[50], last_bg_pid[50];
    sprintf(instance_pid, "%d", getpid());
    setenv("$", instance_pid, 1);
    
    sprintf(last_exit_status, "%d", cmd_exit_status);
    setenv("?", last_exit_status, 1);
    
    sprintf(last_bg_pid, "%d", background_pid);
    setenv("!", last_bg_pid, 1);    
}

/*
 * usage - print a usage message
 */
void usage(void) 
{
    printf("Usage: shell [-x] [-d <level>] [-f file [arg] ... ] \n");
    printf("   -x   print command line after variable substitution\n");
    printf("   -d   print additional diagnostic information\n");
    printf("   -f   output to file\n");
    exit(1);
}


char** eval(char* cmd, int* size)
{
	int argc = 0;
	char **args = malloc(100 * sizeof(char *));
	char *active_input, *pos;
	int i;
	
	if (cmd[0] == '#') 
	{
		args[0] = NULL;
		argc++;
	} 
	else 
	{
		active_input = strtok(cmd, "#");
		
		pos = strchr(active_input, '|');
		pipe_flag = (pos == NULL) ? 0 : 1;
		
		if (pipe_flag) 
		{
			args[argc] = strtok(active_input, "|");
			while (args[argc] != NULL) 
			{
				args[++argc] = strtok(NULL, "|");
			}
		} 
		else 
		{
			pos = strchr(active_input, '&');
			background = (pos == NULL) ? 0 : 1;
			active_input = strtok(active_input, "&");
		
			/* check for redirection */
			in_file_name = NULL;
			out_file_name = NULL;
		
			pos = strchr(active_input, '>');
			if (pos != NULL) 
			{
				out_file_name = strtok(pos + 1, "");
				outfd = open(out_file_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
				dup2(outfd, STDOUT_FILENO);
				active_input = strtok(active_input, ">");
			} 
		
			pos = strchr(active_input, '<');
			if (pos != NULL) 
			{
				redirect_in = 1;
				in_file_name = strtok(pos + 1, " ");
				infd = open(in_file_name, O_RDWR | O_CREAT, S_IRWXU);
				dup2(infd, STDIN_FILENO);	
				active_input = strtok(active_input, "<");	
			} 
		
			/* split input into words. */		
			args[argc] = strtok(active_input, " ");
			while (args[argc] != NULL) 
			{
				argc++;
				args[argc] = strtok(NULL, " ");
			}
		}
	}

	*size = argc;
	args = realloc(args, (argc + 1) * sizeof(char *));
	args[argc] = NULL;
	
	if (args[0] == NULL) 
	{
		return args;
	}
	
	args = var_substitute(args, argc);
	
	if (display_cmd) 
	{
		printf("Command: ");
		for (i = 0; i < argc; i ++) 
		{
			printf("%s ", args[i]);
		}
		printf("\n");
	}

	return args;
}



int run_cmd(int argc, char *argv[])
{

	char last_bg_pid[50], msg[50];
	
	if (argv[0] == NULL) 
	{	
		/* ignore blank line. */
		return 1;
    } 
    else if (strcmp(argv[0], "exit") == 0) 
    { 
		/* exit the shell. */
        if (argv[1] != NULL)
        {
			if (!isalpha(*argv[1]))
			{ 
				cmd_exit_status = atoi(argv[1]);
			} 
		}
		sprintf(msg, "Program exited with exit status %d\n", cmd_exit_status);
		printf("%s", msg);
		exit_status = cmd_exit_status;
		return 0;
	}
	else if (strcmp(argv[0], "clr") == 0)
    { 
    	/* echo: clear screen. */
        cmd_exit_status = clr();
    } 
    else if (strcmp(argv[0], "echo") == 0 || strcmp(argv[0], "show") == 0 )
    { 
    	/* echo: print all the words after the echo/show command. */
        cmd_exit_status = echo(argv);
    } 
    else if (strcmp(argv[0], "environ") == 0) 
	{
        /* Display all defined environment strings.*/
        cmd_exit_status = print_environ();
    } 
    else if (strcmp(argv[0], "export") == 0) 
	{
        /* Export variable with the given value.*/
        cmd_exit_status = export_var(argc, argv);
    } 
	else if (strcmp(argv[0], "unexport") == 0) 
	{
        /* Unexport variable*/
        cmd_exit_status = unexport_var(argc, argv);
    } 
	else if (strcmp(argv[0], "set") == 0) 
	{
        /* Set variable with given value.*/
        cmd_exit_status = set_var(argc, argv);
    } 
    else if (strcmp(argv[0], "unset") == 0) 
	{ 
		/* do unset */
		cmd_exit_status = unset_var(argc, argv);
	} 
	else if ((strcmp(argv[0], "dir") == 0)) 
    {
    	/* list the working directory.*/
		cmd_exit_status = list_directory(argc, argv);
	} 
    else if ((strcmp(argv[0], "chdir") == 0)) 
    {
    	/* change the working directory.*/
		cmd_exit_status = change_directory(argc, argv);
	}   
	else if ((strcmp(argv[0], "history") == 0)) 
    {
    	/* show history of commands.*/
		cmd_exit_status = show_history(argc, argv);
	}   
	else if ((strcmp(argv[0], "repeat") == 0)) 
    {
    	/* repeat previously executed command*/
		cmd_exit_status = repeat_cmd(argc, argv);
	}  
	else if ((strcmp(argv[0], "kill") == 0)) 
    {
    	/* send signal to process.*/
    	if(argc == 2)
    	{
    		cmd_exit_status = kill_cmd(SIGTERM, (pid_t) atoi(argv[1]));
    	}
    	else if(argc == 3)
    	{
    		char* c = "-";
    		if((strchr(argv[1], *c)))
    		{
    			int sig = atoi(strtok(argv[1], "-"));
    			cmd_exit_status = kill_cmd(sig, (pid_t) atoi(argv[2]));
    		}
    	}
    	else
    	{
    		debug_message("usage: kill [-s signal_name] pid ...");
    	}
	}  
    else if (strcmp(argv[0], "pause") == 0) 
	{
        /* Display all defined environment strings.*/
        cmd_exit_status = pause_cmd();
    } 
    else if (strcmp(argv[0], "wait") == 0) 
    { 
		if (argv[1] == NULL) 
		{
			printf("wait <PID>\n");
			cmd_exit_status = 1;
		} 
		else 
		{
        	cmd_exit_status = wait_cmd((pid_t) atoi(argv[1]));
		}
	} 
	else if ((strcmp(argv[0], "help") == 0)) 
    {
    	/* change the working directory.*/
		cmd_exit_status = show_help(argc, argv);
	}  
	else 
	{
		/* Command not found. Look for external command to run. */
		cmd_exit_status = run_external_cmd(argv);
	}
	
	sprintf(msg, "Command exited with exit status %d", cmd_exit_status);
	debug_message(msg);
	
	sprintf(last_bg_pid, "%d", background_pid);
	setenv("!", last_bg_pid, 1); 
	
	return 1;
} 


int run_external_cmd(char *argv[])
{

	pid_t child_pid;
	int child_status;

	/* Block SIGCHILD signals to avoid race conditions*/
    if(sigemptyset(&mask) != 0) {
        debug_message("sigemptyset error");
        exit(-1);
    }
    if(sigaddset(&mask, SIGCHLD) != 0){
        debug_message("sigaddset error");
        exit(-1);
    }
    if(sigprocmask(SIG_BLOCK, &mask, NULL) != 0){
        debug_message("sigprocmask error");
        exit(-1);
    }
	
	if (pipe_flag) 
	{
		debug_message("Running pipelined command");
		execvp(argv[0], argv);
		printf("Unknown command\n");
		exit(0);
	} 
	else 
	{
		child_pid = fork();
	
		if(child_pid == 0) 
		{
			  if(sigprocmask(SIG_UNBLOCK, &mask, NULL) != 0)
			  {
	          	debug_message("sigprocmask error");
	          	exit(-1);
	          }
	          if(setpgid(0, 0) < 0) 
	          {
              	debug_message("setpgid error");
              	exit(-1);
              }
	          if(execvp(argv[0], argv) < 0) 
	          {
	            printf("%s: Command not found\n", argv[0]);
	            exit(1);
              }

		} 
		else 
		{
			if (background) 
			{
				debug_message("Running command in background");
				waitpid(child_pid, &child_status, WNOHANG);
				background_pid = child_pid;
				child_pids[child_count] = child_pid;
				child_count++;
				if (child_count == child_cap) 
				{
					child_cap *= 2;
					child_pids = realloc(child_pids, child_cap * sizeof(pid_t));
					if (child_pids == NULL) 
					{
						printf("Error reallocating child_pids\n");
						exit(-1);
					}
				}
				printf("[%d]\t%d\n", child_count, child_pid);
			} 
			else 
			{
				fg_pid = child_pid;
				debug_message("Running command in foreground");
				wait(&child_status);
			}
		
			return 0;
		}
	}
	
	return 0;
}


int run_pipe_cmd(int argc, char* argv[])
{
    int status;
    int i = 0;
    int command = 0;
    int size;
    pid_t pid;
    char ** parsed_cmd;

    int pipefds[2 * argc];
    for(i = 0; i < argc; i++)
    {
        if(pipe(pipefds + i*2) < 0) 
        {
            perror("couldn't pipe");
            exit(EXIT_FAILURE);
        }
    }

    int j = 0;
    while(command < argc) 
    {
    	parsed_cmd = eval(argv[command], &size);
        pid = fork();
        if(pid == 0) 
        {
            /*if not last command */
            if(command < argc - 1)
            {
                if(dup2(pipefds[j + 1], 1) < 0)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            /*if not first command&& j!= 2*argc */
            if(j != 0 )
            {
                if(dup2(pipefds[j-2], 0) < 0)
                {
                    perror(" dup2");/*j-2 0 j+1 1 */
                    exit(EXIT_FAILURE);
                }
            }

            for(i = 0; i < 2*argc; i++)
            {
                    close(pipefds[i]);
            }

            pipe_flag = 1;
            run_cmd(size, parsed_cmd);
        } 
        else if(pid < 0)
        {
            perror("error");
            exit(EXIT_FAILURE);
        }
        command++;
        j+=2;
    }
    /**Parent closes the pipes and wait for children*/

    for(i = 0; i < 2 * argc; i++){
        close(pipefds[i]);
    }

    for(i = 0; i < argc + 1; i++)
        wait(&status);
}



int run_file_script(FILE *script)
{
	int scripting = 1;
	int size;
	char **args, cmd[100];
	
	while (scripting && !feof(script)) 
	{
		size = 0;
		redirect_in = 0;
	
		dup2(defin, STDIN_FILENO);
		dup2(defout, STDOUT_FILENO);

        /* get user input */
        fgets(cmd, 100, script);
		
		args = eval(cmd, &size);
		
		if (pipe_flag) 
		{
			scripting = run_pipe_cmd(size, args);
			pipe_flag = 0;
		} 
		else 
		{
			scripting = run_cmd(size, args);
		}
    }

	return cmd_exit_status;
}


char** var_substitute(char** argv, int argc)
{
	int i, local = 0;
	char* ptr;
	for (i = 0; i < argc; i ++) 
	{
		if (argv[i][0] == '$') 
		{
			ptr = argv[i];
			ptr ++;				
			
			/* If not shell variable*/
			if ((argv[i] = getenv(ptr)) == NULL) 
			{
				/* check if local variable*/

				int j;
				for(j = 0; j < 100; ++j)
				{
					if(!variables[j].empty)
					{
						if(!strcmp(ptr, variables[j].key)){
							argv[i] = variables[j].value;
							local = 1;
						}
					}
				}
				/* Else not found */
				if(!local)
					printf("There is no value for variable %s.\n", ptr);
			}
		}
	}
	
	return argv;
}

void debug_message(char* message)
{
	if (debug_level > 0) 
	{
		fprintf(stderr, "%s\n", message);
	}

}


/*****************************
 * Builtin command routines
 *****************************/

int clr(){
	
	char buf[1024];
	char *str;

	tgetent(buf, getenv("TERM"));
	str = tgetstr("cl", NULL);
	fputs(str, stdout);
	return 0;
}


int echo(char* argv[])
{
	int i = 1;
	while(argv[i] != NULL) 
	{
		printf("%s ", argv[i]);
		i++;
	}
	printf("\n");
	return 0;
}


int print_environ()
{
	int i = 0;
	while (environ[i])
	{
        printf("%s\n", environ[i++]);
    }
}


int export_var(int argc, char* argv[])
{
	int i = 0;
	char msg[50];
	if(argc != 3){
		debug_message("Wrong arguments");
		return -1;
	}
	setenv(argv[1], argv[2], 1);    
	sprintf(msg, "Exported variable %s with value %s", argv[1], argv[2]);
	debug_message(msg);

	return 1;
}


int unexport_var(int argc, char* argv[])
{
	int error;
	int i = 0;
	char msg[50];
	if(argc != 2){
		debug_message("Wrong arguments");
		return -1;
	}
	error = unsetenv(argv[1]);    
	if (error == -1) 
	{
		debug_message("Unset failed because of an error with unsetenv()");
		perror("Unset");
		return 1;
	}
	sprintf(msg, "Unexported variable %s", argv[1]);
	debug_message(msg);

	return 1;
}


int set_var(int argc, char* argv[])
{
	if(argc != 3){
		debug_message("Wrong arguments");
		return -1;
	}
	int i = 0;
	int lastEmpty = 0;
	int exists = 0;

	for (i = 0; i < 100; ++i) 
	{
		if (variables[i].empty) 
		{
			lastEmpty = i;
		} 
		else
		{
			/*If already exists, replace value */
			if (!strcmp(variables[i].key, argv[1])) 
			{
				strncpy(variables[i].value, argv[2], 100);
				variables[i].empty = 0;
				exists = 1;
				break;
			} 
		}
	}
	/*Else create new value at last available empty position*/
	if (!exists) 
	{
		strncpy(variables[lastEmpty].key, argv[1], 100);
		strncpy(variables[lastEmpty].value, argv[2], 100);
		variables[lastEmpty].empty = 0;
	}
	return 1;
}


int unset_var(int argc, char* argv[])
{
	if(argc != 2)
	{
		debug_message("Wrong arguments");
		return -1;
	}

	int i = 0;
	for (i = 0; i < 100; ++i) 
	{
		if (!strcmp(variables[i].key, argv[1])) 
		{
			variables[i].empty = 1;
			break;
		}
	}
	return 1;
}


int list_directory(int argc, char* argv[])
{
	DIR *d;
 	struct dirent *dir;
  	d = opendir(".");
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
	    {
	      printf("%s\n", dir->d_name);
	    }
	    closedir(d);
	  }
	  return 0;
}


int change_directory(int argc, char* argv[])
{
	char* home_directory = NULL;
	home_directory = getenv("HOME");
	if (argc == 2)
	{
		/* a path is provided, set the working directory. */
		debug_message("Changing directory to given path");
		setenv("PWD", argv[1], 1);
	}
	else if ((home_directory != NULL) && (argc == 1)) 
	{
		/* a path is not provided, set the working directory to be home directory.*/
		debug_message("Changing directory to home directory");
		setenv("PWD", home_directory, 1);
	}
	else if ((home_directory == NULL) && (argc == 1)) 
	{
		/* the home directory is not set, set the working directory to be home directory.*/
		debug_message("Changing directory to root. Home directory undefined"); 
		setenv("PWD", "/", 1);
	}
	else 
	{
		printf("Incorrect format, should be: %s [path].\n", argv[0]);
		return -1;
	}
	
	chdir(getenv("PWD"));
	return 0;
}


int show_history(int argc, char* argv[])
{
	if(argc > 2)
	{
		debug_message("Wrong history arguments");
		return -1;
	}
	int i = 0;
	int n = 0;
	if(argc == 2)
		n = atoi(argv[1]);
	const char *history_line;

	while(history_get(i) != NULL)
	{
		i++;
	}

	if(n > i || argc == 1)
	{
		int k = 0;
		while(history_get(k) != NULL)
		{
			int j = k + 1;
			history_line = history_get(k)->line;
			printf("%d  %s \n", j, (history_line));
			k++;
		}
	}
	else
	{
		int k = i - n;
		while(history_get(k) != NULL)
		{
			int j = k + 1;
			history_line = history_get(k)->line;
			printf("%d  %s \n", j, (history_line));
			k++;
		}
	}
	return 0;
}


int repeat_cmd(int argc, char* argv[])
{
	int i = 0;
	while(history_get(i) != NULL)
	{
		i++;
	}

	const char *repeated_cmd;
	if(argc == 1)
	{
		repeated_cmd = current_history()->line;
	}
	else if(argc == 2)
	{
		int cmd = atoi(argv[1]);
		if(cmd > i)
		{
			fprintf(stderr, "history has %d\n", i);
			debug_message("Number given to repeat too high");
			return -1;
		}
		else
		{
			repeated_cmd = history_get(cmd - 1)->line;
		}
	}
	else
	{
		debug_message("Wrong number of arguments");		
		return -1;
	}

	if(repeated_cmd)
	{
		int size;
		int i = 0;
		char* command = strdup(repeated_cmd);
		char **args = eval(command, &size);
			
		add_history(repeated_cmd);
		return run_cmd(size, args);
	}
	else
	{
		debug_message("Error repeating command");
		return -1;
	}
	return 0;
}


int kill_cmd(int sig, pid_t pid)
{
	kill(pid, sig);
	return 0;
}


int pause_cmd()
{
	int ch;
	printf ("Paused. Press enter to continue...");
	while ((ch = getchar()) != '\n' && ch != EOF);
}


int wait_cmd(pid_t pid)
{
	int status, error, i, is_valid_pid;
	if (pid == -1 )
	{
		wait(NULL);
	} 
	else 
	{
		for (i = 0; i < child_count; i++) 
		{
			if (child_pids[i] == pid) 
			{
				is_valid_pid = 1;
				break;
			}
		}
		
		if (is_valid_pid) 
		{
			debug_message("Waiting on child process");
			error = waitpid(pid, &status, 0);
		
			if (error == -1) 
			{
				perror("waitpid");
			}
			
			is_valid_pid = 0;
			
			for (i = 0; i < child_count; i++) 
			{
				if (child_pids[i] == pid) 
				{
					is_valid_pid = 1;
				}
				if (is_valid_pid && i < (child_count - 1)) 
				{
					child_pids[i] = child_pids[i + 1];
				}
			}
			
			child_count--;
			child_cap = child_count == 0 ? 1 : child_count;
			
			child_pids = realloc(child_pids, child_cap * sizeof(pid_t));
			if (child_pids == NULL) 
			{
				debug_message("Error reallocating child_pids");
				exit(-1);
			}
		}
		return 1;
	}
	
	return 1;
}


int show_help(int argc, char* argv[]){
	return 0;
}



/*****************************
 * Signal Handling
 *****************************/


void sig_handler(int sig)
{
	char msg[100];
	sprintf(msg, "Sending signal %d\n", sig);
	if (!background) {
		debug_message(msg);
		kill(fg_pid, SIGINT);
	}
	return;
}

void sigchld_handler(int sig)
{
	int status;
    pid_t pid = 1;

    while (pid > 0)
    {
        pid = waitpid(-1, &status, WNOHANG|WUNTRACED);
    }
    return;
}


/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
    	debug_message("Signal error");
    return (old_action.sa_handler);
}




