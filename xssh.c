/*
*
* xssh - a simple toy shell that executes certain described inbuilt functions
*
* Name : Elom Kwame Worlanyo
* Email : elomworlanyo@wustl.edu 
*
*/

#include "xssh.h"
#include "utility.h"

/*
 * main - The shell's main routine 
 */

int main(int argc, char **argv) 
{
    char c;     /* Used to get options */
    char *next_arg, **file_args;    /* File and file arguments */
    char *input, **words;
    int i = 0, j = 0, running = 1, arg_count = 0, index = 0;
   
    /* Init Global Vars */
    background, pipe_flag, debug_level, redirect_in = 0;
    cmd_exit_status, display_cmd = 0;
    background_pid = 0;

    defin = dup(STDIN_FILENO);
    defout = dup(STDOUT_FILENO);
    FILE * input_file = NULL;

    init_env();

    /* Parse the command line */
    while ((c = getopt(argc, argv, "xd:f:")) != EOF) 
    {
        switch (c) 
        {
            case 'x':             /* print command line */
                display_cmd = 1;
            break;
            case 'd':             /* emit additional diagnostic info */
                if(isdigit(optarg[0]))
                {
                    debug_level = atoi(optarg);
                }
                else
                {
                    usage();
                    abort();
                }
            break;
            case 'f':             /* Run script from file*/
                file_args = (char**)malloc(sizeof(char*) * 100);
                index = optind - 1;
                while(index < argc)
                {
                    next_arg = strdup(argv[index]); /* get file name/arg */
                    index++;
                    if(next_arg[0] != '-')
                    { /* check if optarg is next switch */
                        file_args[arg_count++] = next_arg;
                    }
                    else break;
                }
                /*make sure optind starts at next switch*/
                optind = index - 1;
                if (input_file == NULL) 
                {
                    if ((input_file = fopen(file_args[0], "r")) == NULL) 
                    {
                        printf("The file name is invalid.\n");
                        running = 0;
                        exit_status = -1;
                    } 
                    else 
                    {
                        /**
                        TODO: ADD remaining file_args to $1, $2, etc
                        **/
                        int arg_pointer = 1;    
                        while(file_args[arg_pointer] != NULL)
                        {
                            printf("%s \n", file_args[arg_pointer]);
                            arg_pointer++;
                        }
                        /* grab file arguments */   
                        exit_status = run_file_script(input_file);
                        fclose(input_file);
                    }
                }
            break;
            case '?':
            if (optopt == 'd' || optopt == 'f')
              fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
              fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
              fprintf (stderr,
                       "Unknown option character `\\x%x'.\n",
                       optopt);
            running = 0;
            exit_status = -1;
            default:
                usage();
                abort();
        }
    }

    /* Initialize child capacity, child count, and array of child pids */
    child_cap = 1;
    child_count = 0;
    child_pids = malloc(child_cap * sizeof(pid_t));
        
    /* Initialize history. Only last 100 entries need be remembered */
    using_history();
    stifle_history(100);
    
    while (running) 
    {
        i = 0;
        redirect_in = 0;
        pipe_flag = 0;
    
        dup2(defin, STDIN_FILENO);
        dup2(defout, STDOUT_FILENO);
        
        /* get the input from user. */
        input = readline(prompt);
        
        if (strcmp(input, "") == 0) 
        {
            continue;
        }
       
       /** TODO: Modify adding to history if input is repeat **/
        if (strstr(input, "repeat") == NULL) 
        {
            add_history(input);
        }
        
        words = eval(input, &i);
        if (pipe_flag)
        {
            running = run_pipe_cmd(i, words);
            pipe_flag = 0;
        } 
        else 
        {
            running = run_cmd(i, words);
        }
        
    }

    return exit_status;     
}








