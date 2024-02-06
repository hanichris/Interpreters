#include <stdlib.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

/**
 * repl - sets up a REPL.
*/
static void repl()
{
	char line[1024];

	for (;;)
	{
		printf("> ");
		if (!fgets(line, sizeof(line), stdin))
		{
			printf("\n");
			break;
		}
		printf("%s\n", line);
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
	// InterpretResult result = interpret(source);
	printf("%s\n", source);
	free(source);

	// if (result == INTERPRET_COMPILE_ERROR) exit(65);
	// if (result == INTERPRET_RUNTIME_ERROR) exit(70);
	
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
