#include <string.h>

#include "common.h"
#include "scanner.h"

/**
 * struct _scanner - captures the states the scanner keeps track of.
 * @start: pointer marking the beginning of the current lexeme being
 * scanned.
 * @current: pointer to the current character being examined.
 * @line: points to the current line the lexeme is on for error reporting.
*/
typedef struct _scanner
{
	const char* start;
	const char* current;
	int line;
} Scanner;

Scanner scanner;

/**
 * initScanner - initializes the state fields of the scanner struct.
 * @source: pointer to the beginning of the source code to scan.
 * Return: void.
*/
void initScanner(const char* source)
{
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;
}