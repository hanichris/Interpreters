#include <stdlib.h>

#include "chunk.h"
#include "compiler.h"
#include "scanner.h"

#if defined(DEBUG_PRINT_CODE)
#include "debug.h"
#endif // DEBUG_PRINT_CODE



typedef struct _parser
{
	bool hadError;
	bool panicMode;
	Token current;
	Token previous;
} Parser;

/**
 * enum _precedence - defines the precedence levels of the
 * language in order from lowest to highest.
*/
typedef enum _precedence
{
	PREC_NONE,
	PREC_ASSIGNMENT,	// =
	PREC_OR,			// or
	PREC_AND,			// and
	PREC_EQUALITY,		// == !=
	PREC_COMPARISON,	// < > <= >=
	PREC_TERM,			// + -
	PREC_FACTOR,		// * /
	PREC_UNARY,			// ! -
	PREC_CALL,			// . ()
	PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)();

/**
 * struct _rule - defines a single row in the parser table.
 * @prefix: function to compile a prefix expression.
 * @infix: function to compile an infix expression.
 * @precedence: precedence of an infix expression.
*/
typedef struct _rule
{
	ParseFn prefix;
	ParseFn infix;
	Precedence precedence;
} ParseRule;

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
	fprintf(stderr, ": %s.\n", message);
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

/**
 * check - Returns true if the current token has the same
 * type as the given type.
 * @type: the given token type.
 * @Return: boolean value denoting the success of the check
 * operation.
*/
static bool check(TokenType type)
{
	return parser.current.type == type;
}

/**
 * match - helper function to detect the type of the current token.
 * If the current token matches the given token, it is consumed. Else,
 * it is left alone and a `false` is returned from the function. Otherwise,
 * the function returns a `true` value.
 * @type: The given token type to match the current token.
 * @Return: boolean value denoting the success of the operation.
*/
static bool match(TokenType type)
{
	if (!check(type)) return false;
	advance();

	return true;	
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

/**
 * makeConstant - inserts an entry into the constants table.
 * Ensures that not too many constants are present in the table.
 * This is because the `OP_CONSTANT` instruction uses a single
 * byte for the index operand, thus can store and load only
 * upto 256 constants in a chunk.
 * @value: element to insert into the constants table.
 * Return: index of the element in the constants table.
*/
static uint8_t makeConstant(Value value)
{
	int constant = addConstant(currentChunk(), value);
	if (constant > UINT8_MAX)
	{
		error("Too many constants in one chunk");
		return 0;
	}
	return (uint8_t)constant;
	
}

static void emitConstant(Value value)
{
	emitBytes(OP_CONSTANT, makeConstant(value));
}

static void endCompiler()
{
	emitReturn();
	#if defined(DEBUG_PRINT_CODE)
	if (!parser.hadError)
	{
		disassembleChunk(currentChunk(), "code");
	}
	#endif // DEBUG_PRINT_CODE
	
}

static void expression();
static void statement();
static void declaration();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

/**
 * binary - acts as the infix parser for the `TOKEN_PLUS`,
 * `TOKEN_MINUS`, `TOKEN_STAR` and `TOKEN_SLASH` token types.
 * It takes into consideration when parsing the right operand
 * has a precedence level one higher than the binary operator.
*/
static void binary()
{
	TokenType operatorType = parser.previous.type;
	ParseRule* rule = getRule(operatorType);
	parsePrecedence((Precedence)rule->precedence + 1);

	switch (operatorType)
	{
		case TOKEN_BANG_EQUAL: emitBytes(OP_EQUAL, OP_NOT); break;
		case TOKEN_EQUAL_EQUAL: emitByte(OP_EQUAL); break;
		case TOKEN_GREATER: emitByte(OP_GREATER); break;
		case TOKEN_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break;
		case TOKEN_LESS: emitByte(OP_LESS); break;
		case TOKEN_LESS_EQUAL: emitBytes(OP_GREATER, OP_NOT); break;
		case TOKEN_PLUS: emitByte(OP_ADD); break;		
		case TOKEN_MINUS: emitByte(OP_SUBTRACT); break;
		case TOKEN_STAR: emitByte(OP_MULTIPLY); break;
		case TOKEN_SLASH: emitByte(OP_DIVIDE); break;
		default: return;
	}
}

static void literal()
{
	switch (parser.previous.type)
	{
		case TOKEN_FALSE: emitByte(OP_FALSE); break;
		case TOKEN_TRUE: emitByte(OP_TRUE); break;
		case TOKEN_NIL: emitByte(OP_NIL); break;
		default: return;
	}
}

/**
 * grouping - assumes that the initial '(' has already been consumed
 * and recursively calls back into `expression` to compile the
 * expression between the parentheses, the parses the closing ')' at
 * the end.
*/
static void grouping()
{
	expression();
	consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression");
}

/**
 * number - converts the lexeme for a number literal into a
 * `double` and generates the bytecode for it through a utility
 * function.
*/
static void number()
{
	double value = strtod(parser.previous.start, NULL);
	emitConstant(NUMBER_VAL(value));
}

static void string()
{
	emitConstant(OBJ_VAL(copyStringVec(parser.previous.start + 1,
									   parser.previous.length - 2)));
}

/**
 * unary - obtains the unary operator and utilises
 * the `PREC_UNARY` precedence level to permit nested
 * unary expressions to compile the operand and emits the bytecode
 * to perform the negation
*/
static void unary()
{
	TokenType operatorType = parser.previous.type;

	parsePrecedence(PREC_UNARY);

	switch (operatorType)
	{
		case TOKEN_MINUS: emitByte(OP_NEGATE); break;
		case TOKEN_BANG: emitByte(OP_NOT); break;
		default: return;
	}
}

ParseRule rules[] = {
	[TOKEN_LEFT_PAREN] 		= {grouping, NULL, PREC_NONE},
	[TOKEN_RIGHT_PAREN] 	= {NULL, NULL, PREC_NONE},
	[TOKEN_LEFT_BRACE] 		= {NULL, NULL, PREC_NONE},
	[TOKEN_RIGHT_BRACE] 	= {NULL, NULL, PREC_NONE},
	[TOKEN_COMMA] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_DOT] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_MINUS] 			= {unary, binary, PREC_TERM},
	[TOKEN_PLUS] 			= {NULL, binary, PREC_TERM},
	[TOKEN_SEMICOLON] 		= {NULL, NULL, PREC_NONE},
	[TOKEN_SLASH] 			= {NULL, binary, PREC_FACTOR},
	[TOKEN_STAR] 			= {NULL, binary, PREC_FACTOR},
	[TOKEN_BANG] 			= {unary, NULL, PREC_NONE},
	[TOKEN_BANG_EQUAL] 		= {NULL, binary, PREC_EQUALITY},
	[TOKEN_EQUAL] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_EQUAL_EQUAL] 	= {NULL, binary, PREC_EQUALITY},
	[TOKEN_LESS] 			= {NULL, binary, PREC_COMPARISON},
	[TOKEN_LESS_EQUAL] 		= {NULL, binary, PREC_COMPARISON},
	[TOKEN_GREATER] 		= {NULL, binary, PREC_COMPARISON},
	[TOKEN_GREATER_EQUAL] 	= {NULL, binary, PREC_COMPARISON},
	[TOKEN_IDENTIFIER] 		= {NULL, NULL, PREC_NONE},
	[TOKEN_STRING] 			= {string, NULL, PREC_NONE},
	[TOKEN_NUMBER] 			= {number, NULL, PREC_NONE},
	[TOKEN_AND] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_CLASS] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_ELSE] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_FALSE] 			= {literal, NULL, PREC_NONE},
	[TOKEN_FOR] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_FUN] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_IF] 				= {NULL, NULL, PREC_NONE},
	[TOKEN_NIL] 			= {literal, NULL, PREC_NONE},
	[TOKEN_OR] 				= {NULL, NULL, PREC_NONE},
	[TOKEN_PRINT] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_RETURN] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_SUPER] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_THIS] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_TRUE] 			= {literal, NULL, PREC_NONE},
	[TOKEN_VAR] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_WHILE] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_ERROR] 			= {NULL, NULL, PREC_NONE},
	[TOKEN_EOF] 			= {NULL, NULL, PREC_NONE}
};

/**
 * parsePrecedence - starts at the current token and parses any
 * expression at the given precedence level or higher.
*/
static void parsePrecedence(Precedence precedence)
{
	advance();
	ParseFn prefixRule = getRule(parser.previous.type)->prefix;
	if (prefixRule == NULL)
	{
		error("Expect expression");
		return;
	}
	prefixRule();

	while (precedence <= getRule(parser.current.type)->precedence)
	{
		advance();
		ParseFn infixRule = getRule(parser.previous.type)->infix;
		infixRule();
	}
}

/**
 * getRule - returns a pointer to the rule at the given index.
 * @type: token type whose parser functions are to be obtained.
 * Return: the struct of functions relating to the token type.
*/
static ParseRule* getRule(TokenType type)
{
	return &rules[type];
}

/**
 * expression - parse the lowest precedence level and subsumes all
 * of the higher precedence expressions as well.
*/
static void expression()
{
	parsePrecedence(PREC_ASSIGNMENT);
}

/***/
static void expressionStatement()
{
	expression();
	consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
	emitByte(OP_POP);
}

/**
 * printStatement - evaluates an expression and prints the result.
*/
static void printStatement()
{
	expression();
	consume(TOKEN_SEMICOLON, "Expect ';' after value");
	emitByte(OP_PRINT);
}

/**
 * declaration - compiles a single declaration.
*/
static void declaration()
{
	statement();
}

/**
 * statement - process a statement which can either be a
 * `declaration statement` that binds a name to a value or the other
 * kinds of statments like print, control flow etc.
 * The grammar rule is for a statement is as presented:
 * 		`statement` -> `exprStmt`
 * 					| 	`forStmt`
 * 					| 	`ifStmt`
 * 					| 	`printStmt`
 * 					| 	`returnStmt`
 * 					| 	`whileStmt`
 * 					| 	`block` ;
*/
static void statement()
{
	if (match(TOKEN_PRINT))
	{
		printStatement();
	} else {
		expressionStatement();
	}
}


bool compile(const char* source, Chunk* chunk)
{
	initScanner(source);
	compilingChunk = chunk;

	parser.hadError = false;
	parser.panicMode = false;

	advance();
	// support a sequence of declarations.
	while (!match(TOKEN_EOF))
	{
		declaration();
	}
	
	endCompiler();
	return !parser.hadError;
}