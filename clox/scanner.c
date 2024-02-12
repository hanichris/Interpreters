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

/**
 * isAtEnd - Tests to see if the end of the source string has
 * been reached.
 * Return: `True` if at the end of the source string. Otherwise, `False`.
*/
static bool isAtEnd()
{
	return *scanner.current == '\0';
}

/**
 * advance - consumes the current character and returns it.
 * Return: read character.
*/
static char advance()
{
	scanner.current++;
	return *(scanner.current - 1);
}

/**
 * peek - returns the current character;
 * Return: character.
*/
static char peek()
{
	return *scanner.current;
}

static char peekNext()
{
	if (isAtEnd()) return '\0';
	return *(scanner.current + 1);
}

/**
 * match - conditionally consumes a second character. If the current
 * character is the desired one, advance and returns true. Else,
 * returns false to indicate it was not matched.
 * @expected: The character to conditionally consume.
 * Return: bool.
*/
static bool match(char expected)
{
	if (isAtEnd()) return false;
	if (*scanner.current != expected) return false;
	scanner.current++;
	return true;
}

/**
 * makeToken - creates a `Token` for the passed `TokenType`.
 * @type: Type of token passed into the function.
 * Return: The `Token` of the particular `TokenType`.
*/
static Token makeToken(TokenType type)
{
	Token token;
	token.type = type;
	token.start = scanner.start;
	token.length = (int)(scanner.current - scanner.start);
	token.line = scanner.line;
	return token;
}

/**
 * errorToken - creates a token for the error message.
 * @message: error string message.
 * Return: `Token` from the error message.
*/
static Token errorToken(const char* message)
{
	Token token;
	token.type = TOKEN_ERROR;
	token.start = message;
	token.length = (int)strlen(message);
	token.line = scanner.line;
	return token;
}

/**
 * skipWhitespace - advance the scanner past any leading whitespaces.
*/
static void skipWhitespace()
{
	for (;;)
	{
		char c = peek();

		switch (c)
		{
			case ' ':
			case '\t':
			case '\r':
				advance();
				break;

			case '\n':
				scanner.line++;
				advance();
				break;

			case '/':
				if (peekNext() == '/')
				{
					while(peek() != '\n' && !isAtEnd()) advance();
				} else if (peekNext() == '*')
				{
					// consume the '/' and '*' at the beginning of the block comment.
					advance();
					advance();
					do
					{
						advance();
					} while (peek() != '*' && peekNext() != '/' && !isAtEnd());
					// consume the '*' and '/' at the end of the block comment.
					advance();
					advance(); 
				}

			default:
				return;
		}
	}
	
}

/**
 * scanToken - scans a complete Token. When a called, it implies
 * the beginning of a new token that is to be scanned. Checks whether
 * the end of the source code has been reached and generates `EOF Token`
 * if true and stops. Generates an `error Token` if the scanner encounters
 * a character it does not recognize. Otherwise, simply generates a new
 * `Token`.
 * Return: Generated `error Token` | `EOF Token` | `normal Token`.
*/
Token scanToken()
{
	skipWhitespace();
	scanner.start = scanner.current;

	if (isAtEnd()) return makeToken(TOKEN_EOF);

	char c = advance();
	switch (c)
	{
		case '(': return makeToken(TOKEN_LEFT_PAREN);
		case ')': return makeToken(TOKEN_RIGHT_PAREN);
		case '{': return makeToken(TOKEN_LEFT_BRACE);
		case '}': return makeToken(TOKEN_RIGHT_BRACE);
		case ';': return makeToken(TOKEN_SEMICOLON);
		case '.': return makeToken(TOKEN_DOT);
		case '-': return makeToken(TOKEN_MINUS);
		case '+': return makeToken(TOKEN_PLUS);
		case '/': return makeToken(TOKEN_SLASH);
		case '*': return makeToken(TOKEN_STAR);
		case '!': return makeToken(
			match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG
		);
		case '=': return makeToken(
			match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL
		);
		case '<': return makeToken(
			match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS
		);
		case '>': return makeToken(
			match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER
		);
		
		default:
			break;
	}

	return errorToken("Unexpected character.");
	
}