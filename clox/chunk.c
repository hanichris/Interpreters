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
	// chunk->lcount = 0;
	chunk->code = NULL;
	chunk->lines = NULL;
	initValueArray(&chunk->constants);
}

/**
 * writeChunk - Append a new byte to the end of a dynamic array
 * together with its line number in the source code.
 * @chunk: pointer to a struct defining a dynamic array.
 * @byte: fixed-width 8-bit int to append to the end of the array.
 * @line: the source line the byte of code being written came from.
 * Return: void.
*/
void writeChunk(Chunk *chunk, uint8_t byte, int line)
{
	if (chunk->capacity < chunk->count + 1)
	{
		int oldCapacity = chunk->capacity;

		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(
			u_int8_t, chunk->code, oldCapacity, chunk->capacity
		);
		chunk->lines = GROW_ARRAY(
			int, chunk->lines, oldCapacity, chunk->capacity
		);
	}

	chunk->code[chunk->count] = byte;
	// if (chunk->count == 0)
	// {
	// 	chunk->lines[chunk->lcount] = line;
	// 	chunk->lines[chunk->lcount + 1] = 1;
	// } else
	// {
	// 	if (line == chunk->lines[chunk->lcount])
	// 	{
	// 		chunk->lines[chunk->lcount + 1] += 1;
	// 	} else
	// 	{
	// 		chunk->lcount += 2;
	// 		chunk->lines[chunk->lcount] = line;
	// 		chunk->lines[chunk->lcount + 1] = 1;
	// 	}
	// }
	chunk->lines[chunk->count] = line;
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
	FREE_ARRAY(int, chunk->lines, chunk->capacity);
	freeValueArray(&chunk->constants);
	initChunk(chunk);
}
