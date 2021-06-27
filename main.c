#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "init_shell.c"

#define INPUT_LINE_LENGHT 1024

pid_t spawnChild(const char* program, char** arg_list)
{
    pid_t ch_pid = fork();

    if (ch_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (ch_pid == 0) {
        execvp(program, arg_list);
        perror(program);
        exit(EXIT_FAILURE);
    } else {
        // printf("spawned child with pid - %d\n", ch_pid);
        return ch_pid;
    }
}

/**
* Get user input safely
* \return 0 if everything went well, 1 if EOF was read
*/
int input(char *string,int length)
{
    if(fgets(string,length,stdin)!=NULL) {
        strsep(&string, "\n");
        return 0;
    }
    else {
        return 1;
    }
}

/**
* Split string with delimiter
* \return string array with each element of the input string splitted by the delimiter
*/
char** split(const char* string, char* delimiter){


    char** stringArray  = NULL;

    char*  s = strdup(string);
    char*  element    = strtok (s, delimiter);

    int elementsCount = 0;

    while (element!=NULL) {
        stringArray = realloc (stringArray, sizeof (char*) * ++elementsCount);

        if (stringArray == NULL)
        exit (EXIT_FAILURE); // memory allocation failed

        stringArray[elementsCount-1] = element;

        element = strtok (NULL, delimiter);
    }

    stringArray = realloc (stringArray, sizeof (char*) * (elementsCount+1));
    stringArray[elementsCount] = 0;

    return stringArray;

}

/**
* Execute internal command if available
* \return 1 if an internal command was found, 0 if no internal command was found
*/
int internalCommand(char** cmdArray){

    if(strcmp(cmdArray[0],"cd")==0){
        if(cmdArray[1]==NULL){
            printf("Usage: cd DESTINATION\n");
        }
        else {
            chdir(cmdArray[1]);
        }
        return 1;
    }

    else {
        return 0;
    }
}


int main() {

    pid_t child;
    int wstatus;

	init_shell();


	while(1) {
		
        char *user_input=malloc(INPUT_LINE_LENGHT);

        char** cmdArray;

        while(strlen(user_input)==0){
            
            printf("$ ");

            if(input(user_input, INPUT_LINE_LENGHT)==1){ // EOF was sent
                printf("exit\n");
                exit(EXIT_SUCCESS);
            }

            // printf("[%s]\n", user_input);

            cmdArray = split(user_input, " ");

            // for(int i=0; cmdArray[i]!=NULL; i++){
            //     printf("[%s]\n", cmdArray[i]);
            // }

        }

        if(internalCommand(cmdArray)==0){

            child = spawnChild(cmdArray[0], cmdArray);

    	    waitpid(child, &wstatus, WUNTRACED | WCONTINUED);
        
        }

	}
}
