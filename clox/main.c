#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"


/**
 * readline - reads in a line from the standard input stream, delimited
 * by the newline character. 
*/
static int readline(char** buffer)
{
	#define BUFSIZE 1024
	int bufsize = BUFSIZE;
	int idx = 0;
	*buffer = malloc(sizeof(char) * bufsize);
	if (*buffer == NULL)
	{
		fprintf(stderr, "Error: Not enough memory to allocate to read a line.\n");
		exit(74);
	}

	while (true)
	{
		// Read a character from the standard input.
		int c = fgetc(stdin);

		// If a newline character is encountered, replace it
		// with the null character and break out of the infinite loop.
		if (c == '\n')
		{
			*(*buffer + idx) = '\n';
			idx++;
			*(*buffer + idx) = '\0';
			break;
		} else if (c == EOF)
		{
			free(*buffer);
			return -1;
		}
		
		*(*buffer + idx) = c;
		idx++;

		// If the buffer is exceeded, reallocate more memory for it.
		if (idx >= bufsize)
		{
			bufsize += BUFSIZE;
			*buffer = realloc(*buffer, bufsize);
			if (*buffer == NULL)
			{
				fprintf(stderr, "Error: Not enough memory to allocate to read a line.\n");
				exit(74);
			}
		}
	}

	#undef BUFSIZE
	return idx;
}

/**
 * repl - sets up a REPL.
*/
static void repl()
{
	char* line;

	for (;;)
	{
		printf("> ");
		if (readline(&line) == -1)
		{
			printf("\n");
			break;
		}
		printf("String length: %lu\n", strlen(line));
		free(line);
		// interpret(line);
	}
	
}

/**
 * readFile - reads in to memory the contents of the binary file.
 * @path: path to the binary file.
 * Return: pointer to the buffer holding the contents of the file
 * in memory.
*/
static char* readFile(const char* path)
{
	FILE* fptr = fopen(path, "rb");

	if (fptr == NULL)
	{
		fprintf(stderr, "Error: Could not open file \"%s\".\n", path);
		exit(74);
	}

	fseek(fptr, 0L, SEEK_END);
	size_t fileSize = ftell(fptr);
	rewind(fptr);

	char* buffer = (char *)malloc(fileSize + 1);
	if (buffer == NULL)
	{
		fprintf(stderr, "Error: Not enough memory to read \"%s\".\n", path);
		exit(74);
	}

	size_t bytesRead = fread(buffer, sizeof(char), fileSize, fptr);
	if (bytesRead < fileSize)
	{
		fprintf(stderr, "Error: Could not read file \"%s\". \n", path);
		exit(74);
	}
	
	buffer[bytesRead] = '\0';

	fclose(fptr);
	return buffer;
}

/**
 * runFile - reads a file and executes the resulting string.
 * Based on the result of the execution, the appropriate exit
 * code is set.
 * @path: Path to file that is to be executed.
 * Return: void.
*/
static void runFile(const char* path)
{
	char* source = readFile(path);
	InterpretResult result = interpret(source);
	free(source);

	if (result == INTERPRET_COMPILE_ERROR) exit(65);
	if (result == INTERPRET_RUNTIME_ERROR) exit(70);
	
}

int main(int argc, char **argv)
{
	initVM();

	// if no arguments are passed, drop into REPL mode.
	if (argc == 1)
	{
		repl();
	} else if (argc == 2)
	{
		// A single line command line argument for the filepath to run.
		runFile(argv[1]);
	} else
	{
		fprintf(stderr, "Usage: clox [path]\n");
		exit(64);
	}
	
	freeVM();
	return (0);
}
