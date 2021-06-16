#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "init_shell.c"

int main(int argc, char *argv[]) {

	init_shell();

	char user_input[255];

	while(strcmp(user_input,"quit")){
		
		scanf("%[^\n]%*c", user_input);
		printf("%s\n", user_input);
	}
}