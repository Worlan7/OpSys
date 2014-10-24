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
    else if ((strcmp(argv[0], "chdir") == 0)) 
    {
    	/* change the working directory.*/
		cmd_exit_status = change_directory(argc, argv);
	} 
	else if (strcmp(argv[0], "set") == 0) 
	{
        /* Set variable by the given value.*/
        cmd_exit_status = set_var(argc, argv);
    } 
    else if (strcmp(argv[0], "unset") == 0) 
	{ 
		/* do unset */
		cmd_exit_status = unset_var(argc, argv);
	} 
    else if (strcmp(argv[0], "environ") == 0) 
	{
        /* Display all defined environment strings.*/
        cmd_exit_status = print_environ();
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
			execvp(argv[0], argv);
			printf("Unknown command\n");
			exit(0);
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
	int i, size;
	pid_t pid;
	char **command;
	int **pipes;
	pipes = (int**)malloc(argc*sizeof(int*));

	/* initialize pipes */
	for(i=0;i<argc;i++)
	{
	    pipes[i] = (int*)malloc(2*sizeof(int));
	}
	
	/* Create argc child processes. */
	for (i = 0; i < argc; i ++) 
	{
		if (i < argc - 1) 
		{
			if ((pipe(pipes[i])) < 0) 
			{
				perror("Failed pipe setup.");
			}
		}
		
		if((pid = fork()) < 0) 
		{
			perror("Failed fork.");
		}
		
		if (pid == 0) 
		{			
			command = eval(argv[i], &size);
			
			if (i == 0) 
			{
				close(pipes[i][0]);
				dup2(pipes[i][1], STDOUT_FILENO);
				close(pipes[i][1]);
			} 
			else if (i == argc - 1) 
			{
				close(pipes[i - 1][1]);
				dup2(pipes[i - 1][0], STDIN_FILENO);
				close(pipes[i - 1][0]);
			} 
			else 
			{
				close(pipes[i - 1][1]);
				close(pipes[i][0]);
				dup2(pipes[i - 1][0], STDIN_FILENO);
				close(pipes[i - 1][0]);
				dup2(pipes[i][1], STDOUT_FILENO);
				close(pipes[i][1]);
			}
			pipe_flag = 1;
			run_cmd(size, command);
		}
		else 
		{
			/* parent process*/
			waitpid(pid, NULL, WNOHANG);
		}
	}
	return 1;

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
	int i;
	char* ptr;
	for (i = 0; i < argc; i ++) 
	{
		if (argv[i][0] == '$') 
		{
			ptr = argv[i];
			ptr ++;				
			
			if ((argv[i] = getenv(ptr)) == NULL) 
			{
				printf("There is no environment value for variable %s.\n", ptr);
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

int clr(){
	
	char buf[1024];
	char *str;

	tgetent(buf, getenv("TERM"));
	str = tgetstr("cl", NULL);
	fputs(str, stdout);
	return 0;
}


int wait_cmd(pid_t pid)
{
	return 0;
}


int set_var(int argc, char* argv[])
{
	return 0;
}


int unset_var(int argc, char* argv[])
{
	return 0;
}


int export_var()
{

}


int unexport_var()
{

}


int print_environ()
{
	int i = 0;
	while (environ[i])
	{
        printf("%s\n", environ[i++]);
    }
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
