#include "array_utils.c"

void add_process(job* j, char** cmd_array){

	struct process* p = malloc(sizeof(struct process));
	j->first_process = p;

	
	p->next = NULL;
	p->argv = cmd_array;

}

void create_job(job* j, char** cmd_array){

    int cmd_output = STDOUT_FILENO;
    int cmd_input = STDIN_FILENO;

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

    j->next = NULL;
    j->command = cmd_array[0];
    j->pgid = 0;
    j->notified = 'f';
    j->stdin = cmd_input;
    j->stdout = cmd_output;
    j->stderr = STDERR_FILENO;

    add_process(j, cmd_array);
}

