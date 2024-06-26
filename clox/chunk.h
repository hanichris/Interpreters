#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

/**
 * enum opcode - defines the various opcodes of the bytecode.
*/
typedef enum opcode
{
	OP_CONSTANT,
	OP_NIL,
	OP_TRUE,
	OP_FALSE,
	OP_EQUAL,
	OP_GREATER,
	OP_LESS,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_NOT,
	OP_NEGATE,
	OP_PRINT,
	OP_JUMP,
	OP_JUMP_IF_FALSE,
	OP_POP,
	OP_GET_LOCAL,
	OP_SET_LOCAL,
	OP_GET_GLOBAL,
	OP_DEFINE_GLOBAL,
	OP_SET_GLOBAL,
	OP_RETURN
} OpCode;


/**
 * struct ar - structure to define a dynamic array.
 * @count: Number of entries currently in the array.
 * @capacity: Current size of the dynamic array.
 * @code: pointer to an array that will vary in size.
 * @lines: pointer to an array whose elements are the
 * corresponding line numbers in the bytecode.
 * @constants: store the constants within a chunk
*/
typedef struct ar
{
	int count;
	int capacity;
	uint8_t *code;
	int* lines;
	// int lcount;
	ValueArray constants;
} Chunk;


void initChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, int line);
int addConstant(Chunk *chunk, Value value);
int findConstant(Chunk *chunk, Value value);
void freeChunk(Chunk *chunk);

#endif // clox_chunk_h
