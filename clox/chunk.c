#include <stdlib.h>

#include "chunk.h"
#include "memory.h"


/**
 * initChunk - initialize a dynamic array with some defaults.
 * Also initializes the chunk's constant list.
 * @chunk: pointer to a struct defining a dynamic array.
 * Return: void
*/
void initChunk(Chunk *chunk)
{
	chunk->count = 0;
	chunk->capacity = 0;
	chunk->code = NULL;
	initValueArray(&chunk->constants);
}

/**
 * writeChunk - Append a new byte to the end of a dynamic array.
 * @chunk: pointer to a struct defining a dynamic array.
 * @byte: fixed-width 8-bit int to append to the end of the array.
 * Return: void.
*/
void writeChunk(Chunk *chunk, uint8_t byte)
{
	if (chunk->capacity < chunk->count + 1)
	{
		int oldCapacity = chunk->capacity;

		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(
			u_int8_t, chunk->code, oldCapacity, chunk->capacity);
	}

	chunk->code[chunk->count] = byte;
	chunk->count++;

}

/**
 * addConstant - A convenience method to add a new constant to a chunk.
 * Afterwards, returns the index where the constant was added to aid in
 * the constants retrieval.
 * @chunk: pointer to a struct defining a dynamic array.
 * @value: Constant value to be added to the dynamic array's list of constants.
 * Return: Index where the constant was added to.
*/
int addConstant(Chunk *chunk, Value value)
{
	writeValueArray(&chunk->constants, value);
	return chunk->constants.count - 1;
}

/**
 * freeChunk - deletes the allocated dynamic array.
 * @chunk: pointer to a structure defining a dynamic array.
*/
void freeChunk(Chunk *chunk)
{
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	freeValueArray(&chunk->constants);
	initChunk(chunk);
}
