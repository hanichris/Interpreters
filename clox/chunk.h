#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

/**
 * enum opcode - defines the various opcodes of the bytecode.
 * @OP_RETURN: return instruction operation code.
*/
typedef enum opcode
{
	OP_RETURN
} OpCode;


/**
 * struct ar - structure to define a dynamic array.
 * @count: Number of entries currently in the array.
 * @capacity: Current size of the dynamic array.
 * @code: pointer to an array that will vary in size.
 * @constants: store the constants within a chunk
*/
typedef struct ar
{
	int count;
	int capacity;
	u_int8_t *code;
	ValueArray constants;
} Chunk;


void initChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, u_int8_t byte);
int addConstant(Chunk *chunk, Value value);
void freeChunk(Chunk *chunk);

#endif // clox_chunk_h
