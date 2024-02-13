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
 * isAlpha - Tests whether the character is an ASCII letter or an underscore.
 * @c: character whose value is being investigated.
 * Return: bool.
*/
static bool isAlpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

/**
 * isDigit - Tests whether the character is a number between `0` and `9`
 * inclusive.
 * @c: character whose value is being investigated.
 * Return: bool.
*/
static bool isDigit(char c)
{
	return c >= '0' && c <= '9';
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

/**
 * peekNext - returns the character after the current one.
 * Return: character.
*/
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
						if (peek() == '\n') scanner.line++;
						
						advance();
					} while ((peekNext() != '/' && peek() != '*') && !isAtEnd());
					// consume the '*' and '/' at the end of the block comment.
					advance();
					advance(); 
				} else
				{
					return;
				}
				break;

			default:
				return;
		}
	}
	
}

/**
 * checkKeyword - determines if the portion of the string being
 * processed matches a reserved keyword. If it does, returns the
 * appropriate `TokenType` for that keyword else returns a
 * `TokenType` of a user-defined identifier.
 * @start: point to start processing the partial string from.
 * @length: the length of the partial string.
 * @rest: the partial string to test against.
 * @type: the `TokenType` to be determined.
 * Return: the appropriate token type.
*/
static TokenType checkKeyword(int start, int length,
const char* rest, TokenType type)
{
	if (scanner.current - scanner.start == start + length &&
		memcmp(scanner.start + start, rest, length) == 0)
	{
			return type;
	}
	return TOKEN_IDENTIFIER;
	
}

static TokenType identifierType()
{
	switch (scanner.start[0])
	{
		case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
		case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
		case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
		case 'f':
			if (scanner.current - scanner.start > 1)
			{
					switch (scanner.start[1])
					{
						case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
						case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
						case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
					}
			}
			break;
		case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
		case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
		case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
		case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
		case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
		case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
		case 't':
			if (scanner.current - scanner.start > 1)
			{
				switch (scanner.start[1])
				{
					case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
					case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
				}
			}
			break;
		case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
		case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
	}
	return TOKEN_IDENTIFIER;
}

static Token identifier()
{
	while(isAlpha(peek()) || isDigit(peek())) advance();
	return makeToken(identifierType());
}

static Token number()
{
	while (isDigit(peek())) advance();

	// Look for a fractional part in the number.
	if (peek() == '.' && isDigit(peekNext()))
	{
		// Consume the '.'.
		advance();
		while (isDigit(peek())) advance();
	}
	return makeToken(TOKEN_NUMBER);
}

static Token string()
{
	while (peek() != '"' && !isAtEnd())
	{
		if (peek() == '\n') scanner.line++;
		advance();
	}

	if (isAtEnd()) return errorToken("Unterminated string");
	// Consume the closing quote.
	advance();
	return makeToken(TOKEN_STRING);
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
	if (isAlpha(c)) return identifier();
	if (isDigit(c)) return number();
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
		case '"': return string();
		
		default:
			break;
	}

	return errorToken("Unexpected character.");
	
}