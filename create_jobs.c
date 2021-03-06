#include "array_utils.c"

process* create_process(char** args_array){

    if(internalCommand(args_array)==0){

        struct process* p = malloc(sizeof(struct process));
        p->argv = args_array;
        p->next = NULL;
        return p;
    }

    else {
        return NULL;
    }
}

void add_process(job* j, char** cmd_array){

    j->first_process = create_process(split(cmd_array[0], " "));

    process* previous_process = j->first_process;

    for (int i = 1; cmd_array[i]!=NULL; i++)
    {
        previous_process->next = create_process(split(cmd_array[i], " "));
        previous_process = previous_process->next;
    }

}

/**
* Redirect input and output of job if needed
* \return command string without file redirections symbols
*/
char* redirect_input_output(job* j, char* cmd){
    int cmd_output = STDOUT_FILENO;
    int cmd_input = STDIN_FILENO;

    char** cmd_array = split(cmd, " ");

    for(int i=0; cmd_array[i]!=NULL; i++){

        // Redirect output
        if (strcmp(cmd_array[i], ">")==0){
            if (cmd_array[i+1]==0){
                printf("Please specify a filename to redirect output, ignoring\n");
                cmd_array[i]=0;
            }
            else {
                cmd_output = open(cmd_array[i+1], O_WRONLY | O_CREAT, 0755);
                remove_array_elements(cmd_array, i, 2);
                i--;
            }
        }

        // Redirect input
        if (strcmp(cmd_array[i], "<")==0){
            if (cmd_array[i+1]==0){
                printf("Please specify a filename to redirect input, ignoring\n");
                cmd_array[i]=0;
            }
            else {
                cmd_input = open(cmd_array[i+1], O_RDONLY, 0200);
                remove_array_elements(cmd_array, i, 2);
                i--;
            }
        }

    }

    j->stdin = cmd_input;
    j->stdout = cmd_output;
    j->stderr = STDERR_FILENO;

    return array_to_string(cmd_array, " ");
}

void create_job(job* j, char* user_input){

    j->next = NULL;
    j->pgid = 0;
    j->notified = 'f';
    
    char* cmd = redirect_input_output(j, user_input);

    char** cmd_array = split(cmd, "|");
    add_process(j, cmd_array);

}

