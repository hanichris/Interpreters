#include <stdlib.h>

#include "chunk.h"
#include "compiler.h"
#include "scanner.h"


typedef struct _parser
{
	bool hadError;
	bool panicMode;
	Token current;
	Token previous;
} Parser;

Parser parser;
Chunk* compilingChunk;

static Chunk* currentChunk()
{
	return compilingChunk;
}


/**
 * errorAt - Prints where the error occurred and tries to show
 * the lexeme if it is human-readable. It the prints the error
 * message and sets the `hadError` flag of the parser which records
 * whether errors occurred during compilation.
 * @token: problematic token.
 * @message: error message to print out.
*/
static void errorAt(Token* token, const char* message)
{
	if (parser.panicMode) return;
	
	parser.panicMode = true;
	fprintf(stderr, "[line %d] Error", token->line);

	if (token->type == TOKEN_EOF)
	{
		fprintf(stderr, " at end");
	} else if (token->type == TOKEN_ERROR)
	{
		/* code */
	} else
	{
		fprintf(stderr, " at '%.*s'", token->length, token->start);
	}
	fprintf(stderr, ": %s\n", message);
	parser.hadError = true;
}

/**
 * error - Reports an error at the location of the token just consumed.
 * @message: Message to show to the user.
*/
static void error(const char* message)
{
	errorAt(&parser.previous, message);
}

/**
 * errorAtCurrent - Reports an error token to the user. Extracts
 * the location out of the current token in order to tell the user
 * where it occurred.
 * @message: Message to show the user.
*/
static void errorAtCurrent(const char* message)
{
	errorAt(&parser.current, message);
}

/**
 * advance - steps forward through the input stream asking
 * the scanner for the next Token which is stored for later
 * use. It keeps looping, reading tokens, reporting errors
 * until it hits a non-error one or reaches the end.
*/
static void advance()
{
	parser.previous = parser.current;

	for (;;)
	{
		parser.current = scanToken();
		if (parser.current.type != TOKEN_ERROR) break;
		errorAtCurrent(parser.current.start);
	}
	
}

/**
 * consume - reads the next token and validates that it has the
 * expected type. If not it reports an error.
 * @type: The expected `TokenType`.
 * @message: The error message to display to the user.
*/
static void consume(TokenType type, const char* message)
{
	if (parser.current.type == type)
	{
		advance();
		return;
	}
	errorAtCurrent(message);
}

static void emitByte(uint8_t byte)
{
	writeChunk(currentChunk(), byte, parser.previous.line);
}

/**
 * emitBytes - convenience function to write an opcode followed
 * by a one-byte operand.
 * @byte1: opcode.
 * @byte2: operand.
*/
static void emitBytes(uint8_t byte1, uint8_t byte2)
{
	emitByte(byte1);
	emitByte(byte2);
}

static void emitReturn()
{
	emitByte(OP_RETURN);
}

static void endCompiler()
{
	emitReturn();
}

bool compile(const char* source, Chunk* chunk)
{
	initScanner(source);
	compilingChunk = chunk;

	parser.hadError = false;
	parser.panicMode = false;

	advance();
	expression();
	consume(TOKEN_EOF, "Expect end of expression.");
	endCompiler();
	return !parser.hadError;
}