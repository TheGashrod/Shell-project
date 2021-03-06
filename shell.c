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

    if(strcmp(cmd_array[0],"mkdir")==0){

        if(cmd_array[1]==NULL){
            printf("Usage: mkdir PATH\n");
        }
        else {
            mkdir(cmd_array[1], 755);
        }
        return 1;

    }

    if(strcmp(cmd_array[0],"cp")==0){

        if(cmd_array[1]==NULL || cmd_array[2]==NULL){
            printf("Usage: cp SOURCE DESTINATION \n");
        }
        else {
            copyElement(cmd_array[1], cmd_array[2]);
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