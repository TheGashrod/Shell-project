#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/stat.h>
#include <fcntl.h>

#include "init_shell.c"
#include "launch_jobs.c"

#define INPUT_LINE_LENGHT 1024
#define PROMPT_COLOR "[1;32m" // Bold green

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
int internalCommand(char** cmd_array){

    if(strcmp(cmd_array[0],"cd")==0){
        if(cmd_array[1]==NULL){
            printf("Usage: cd PATH\n");
        }
        else {
            chdir(cmd_array[1]);
        }
        return 1;
    }

    else {
        return 0;
    }
}

/**
* Print prompt text (usually when asking user input)
*/
void displayPromptText(){

    char current_working_directory[100];
    getcwd(current_working_directory, 100);

    printf("\033%s", PROMPT_COLOR); //Set the text to the color green
    
    printf("[%s]$ ", current_working_directory);

    printf("\033[0m"); //Resets the text to default color
}

/**
* Remove count elements of the array starting from position
*/
void remove_array_elements(char** array, int position, int count){
    

    while(array[position+count]!=NULL){
        array[position] = array[position+count];
        position++;
    }

    array[position]=0;

}

int main() {

	init_shell();

	while(1) {
		
        char *user_input=malloc(INPUT_LINE_LENGHT);

        char** cmd_array;

        while(strlen(user_input)==0){

            displayPromptText();

            if(input(user_input, INPUT_LINE_LENGHT)==1){ // EOF was sent
                printf("exit\n");
                exit(EXIT_SUCCESS);
            }

            // printf("[%s]\n", user_input);

            cmd_array = split(user_input, " ");

            // for(int i=0; cmd_array[i]!=NULL; i++){
            //     printf("[%s]\n", cmd_array[i]);
            // }

        }

        if(internalCommand(cmd_array)==0){

            struct process p = {
                .argv = cmd_array
            };

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

            struct job j = {
                .next = NULL,
                .command = cmd_array[0],
                .first_process = &p,
                .pgid = 0,
                .notified = 'f',
                .stdin = cmd_input,
                .stdout = cmd_output,
                .stderr = STDERR_FILENO
            };

            launch_job(&j, 1);
        
        }

	}
}
