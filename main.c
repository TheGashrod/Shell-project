#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/stat.h>
#include <fcntl.h>

#include "cp.c"
#include "init_shell.c"
#include "launch_jobs.c"
#include "shell.c"
#include "create_jobs.c"

#define INPUT_LINE_LENGHT 1024

int main() {

	init_shell();


	while(1) {
		
        char *user_input=malloc(INPUT_LINE_LENGHT);

        while(strlen(user_input)==0){

            displayPromptText();

            if(input(user_input, INPUT_LINE_LENGHT)==1){ // EOF was sent
                printf("exit\n");
                exit(EXIT_SUCCESS);
            }

        }

        // printf("[%s]\n", user_input);

        // for(int i=0; cmd_array[i]!=NULL; i++){
        //     printf("[%s]\n", cmd_array[i]);
        // }
        
        struct job j;

        int is_foreground = 1;


        if(*user_input=='&'){
            is_foreground = 0;
            user_input = user_input+1;
        }


        create_job(&j, user_input);

        if(j.first_process!=NULL)
            launch_job(&j, is_foreground);

	}
}
