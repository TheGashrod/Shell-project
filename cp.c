#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define PAGESIZE 4096

void copyElement(const char* input_path, const char* output_path);

/**
* Concatenate string a and b into a new string.
*/
char* concatenate(const char* a, const char* b){
    // A string in C is just a pointer to an array of char that is terminated by the first null character.
    char* result = malloc(strlen(a) + strlen(b) + 1); // +1 for the null-terminator
    if (!result) {
        fprintf(stderr, "malloc() failed: insufficient memory!\n");
        exit(EXIT_FAILURE);
    }
    strcpy(result, a);
    strcat(result, b);
    return result;
}

/**
* Copy file located at input_file_name to output_file_name
*/
void copyFile(const char *input_file_name, const char* output_file_name){

    int input_file_descriptor = open(input_file_name, O_RDONLY);

    if (input_file_descriptor==-1) {
        perror(input_file_name); // Error with input file
        exit(EXIT_FAILURE);
    }

    struct stat istat;
    fstat(input_file_descriptor, &istat);

    errno = 0;
    int output_file_descriptor = open(output_file_name, O_WRONLY | O_CREAT | O_EXCL, 0666);

    if (output_file_descriptor==-1){
        perror(output_file_name);

        if(errno==EEXIST) {
            // Output file exists

            printf("Do you want to override this file ? (y/n):");

            char choice;

            while ((choice = getchar()) != 'n' && choice != 'y');

            if(choice=='y'){
                // if the user choosed to continue we open the existing output file to replace its content
                output_file_descriptor = open(output_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666); 
            }
            else {
                exit(EXIT_FAILURE);
            }
        }
        else {
            exit(EXIT_FAILURE);
        }
    }

    printf("%s --> %s \n", input_file_name, output_file_name);

    // Copy file block by block with block size corresponding to page size (defined at the beginning)
    while(1){
        char buffer[PAGESIZE];

        int read_count = read(input_file_descriptor, buffer, PAGESIZE);

        if(read_count == 0)
            break; // EOF reached

        int buffer_position = 0;

        while(read_count!=0){ // while the read block is not entirely written

            // in case write was interrupted we save the written write_count 
            int write_count = write(output_file_descriptor, buffer+buffer_position, read_count);
            
            // we obtain the remaining read_count to write 
            read_count -= write_count;
            
            // and the buffer_position to write from
            buffer_position += write_count;
        }
    }

    // Applying the same permissions than the input file to the output file
    fchmod(output_file_descriptor, istat.st_mode);

    if(close(input_file_descriptor)==-1)
        perror(input_file_name);

    if(close(output_file_descriptor)==-1)
        perror(output_file_name);

}

/**
* Copy directory content of input_directory_name to output_directory_name
*/
void copyDirectory(const char* input_directory_name, const char* output_directory_name){

    DIR *input_directory_stream;
    struct dirent *input_directory_entry;

    if ((input_directory_stream = opendir(input_directory_name)) == NULL) {
        perror(input_directory_name);
        exit(EXIT_FAILURE);
    }


    while ((input_directory_entry = readdir(input_directory_stream)) != NULL ){
        if (strcmp(input_directory_entry->d_name, ".") && strcmp(input_directory_entry->d_name, "..")) { 
            // Do not copy . and .. directories

            // Add current directory name to origin and destination path of the entry to copy
            char* origin_path = concatenate(input_directory_name, input_directory_entry->d_name);

            char* destination_path = concatenate(output_directory_name, input_directory_entry->d_name);
            
            copyElement(origin_path, destination_path);
            
            // To avoid leaking memory due to the malloc in concatenate function
            free(origin_path);
            free(destination_path);
        }
    }

    if (closedir(input_directory_stream) == -1)
        perror(input_directory_name);
}

/**
* Copy the element (file or directory) at input_path to output path
*/
void copyElement(const char* input_path, const char* output_path){
    struct stat input_stat;
    stat(input_path, &input_stat);

    if (S_ISDIR(input_stat.st_mode)) {
        // input path is a directory

        // Creating output directory
        // if output directory already exists mkdir will fail but it will not be a problem
        mkdir(output_path, 0700);

        // Applying the same permissions than the input directory to the output directory
        chmod(output_path, input_stat.st_mode);

        // If the directory names does not end with '/' add it
        if(input_path[strlen(input_path)-1]!='/')
            input_path = concatenate(input_path, "/");

        if(output_path[strlen(output_path)-1]!='/')
            output_path = concatenate(output_path, "/");

        copyDirectory(input_path, output_path);
        
    }
    else {
        // Input is a file
        copyFile(input_path, output_path);
    }
}

/**
* Execute tests
*/
void tests(){
    printf("---------------- TESTS ----------------\n");
    printf("----------------------------------------\n");
    printf("-#- Copy single file\n");
    printf("Creating file...\n");
    system("dd ibs=1k count=16k if=/dev/urandom of=singleTestFile");
    printf("Copy...\n");
    copyElement("singleTestFile", "singleTestFileCopy");
    printf("Comparing files...\n");
    system("cmp --silent singleTestFile singleTestFileCopy && echo '### SUCCESS! ###' || echo '### FAILED! ###'");
    printf("Removing files...\n");
    system("rm singleTestFile*");

    printf("----------------------------------------\n");
    printf("-#- Copy directory content recursively \n");
    printf("Creating files...\n");
    system("mkdir testDir1");
    system("dd ibs=1k count=1k if=/dev/urandom of=testDir1/file1");
    system("mkdir testDir1/testDir2");
    system("dd ibs=1k count=1k if=/dev/urandom of=testDir1/testDir2/file2");
    system("mkdir testDir1/testDir2/testDir3");
    system("dd ibs=1k count=1k if=/dev/urandom of=testDir1/testDir2/testDir3/file3");
    printf("Copy...\n");
    copyElement("testDir1", "testDir1Copy/");
    printf("Comparing files...\n");    
    char *testCmd = "cmp -s testDir1/file1 testDir1Copy/file1";
    testCmd = concatenate(testCmd, " && cmp -s testDir1/testDir2/file2 testDir1Copy/testDir2/file2");
    testCmd = concatenate(testCmd, " && cmp -s testDir1/testDir2/testDir3/file3 testDir1Copy/testDir2/testDir3/file3");
    testCmd = concatenate(testCmd, " && echo '### SUCCESS! ###' || echo '### FAILED! ###'");
    system(testCmd);
    free(testCmd);
    printf("Removing files...\n");
    system("rm -r testDir*");

    exit(EXIT_SUCCESS);
}

/**
* Main function
*/
void copy(int argc, char const *argv[])
{
    if (argc == 2 && strcmp(argv[1],"--tests")==0){
        // Run tests
        tests();
    }

    else if (argc < 3)
    {
        // Not enough arguments
        fprintf(stderr, "Usage: %s SOURCE DESTINATION\n", argv[0]);
        fprintf(stderr, "To run tests: %s --tests\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    else {
        printf("Copying %s ", argv[1]);
        printf("to %s\n", argv[2]);

        copyElement(argv[1], argv[2]);

        exit(EXIT_SUCCESS);
    }
}