# define some Makefile variables for the compiler and compiler flags
CC = gcc
CFLAGS  = -g -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Werror -pedantic
BUILD_PATH = bin

default: shell

shell:  main.o 
	$(CC) $(CFLAGS) -o $(BUILD_PATH)/shell $(BUILD_PATH)/main.o

main.o: main.c
	$(CC) $(CFLAGS) -c -o $(BUILD_PATH)/main.o main.c

clean: 
	$(RM) $(BUILD_PATH)/*