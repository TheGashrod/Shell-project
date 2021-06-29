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
* Separate array in two parts delimited by delimiter string
* pointer to the begining of the second part is stored in array
* \return pointer to the first part
*/
char** array_sep(char** array, char* delimiter){

    char** first_array = array;

    for (int i = 0; array[i]!=NULL; ++i){
        if(strcmp(array[i], delimiter)==0){
            array[i]=0;
            array=&array[i+1];
            break;
        }
    }

    return first_array;
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

/**
* Return a string of the array elements separated by delimiter
*/
char* array_to_string(char** array, char* delimiter){

    int string_size = 0;
    char* string = malloc(string_size);

    for (int i = 0; array[i]!=NULL; i++){

        string_size = string_size + sizeof(char*) * strlen(array[i]) + 1;
        string = realloc(string, string_size );

        strcat(string, array[i]);
        strcat(string, delimiter);
    }

    return string;
}