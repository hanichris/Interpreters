#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

/**
 * enum opcode - defines the various opcodes of the bytecode.
 * @OP_CONSTANT: 'load constant' operation code.
 * @OP_RETURN: 'return instruction' operation code.
*/
typedef enum opcode
{
	OP_CONSTANT,
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
	u_int8_t *code;
	int* lines;
	// int lcount;
	ValueArray constants;
} Chunk;


void initChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, u_int8_t byte, int line);
int addConstant(Chunk *chunk, Value value);
void freeChunk(Chunk *chunk);

#endif // clox_chunk_h
