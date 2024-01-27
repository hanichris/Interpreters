#include <stdlib.h>

#include "chunk.h"
#include "memory.h"


/**
 * initChunk - initialize a dynamic array with some defaults.
 * @chunk: pointer to a struct defining a dynamic array.
 * Return: void
*/
void initChunk(Chunk *chunk)
{
	chunk->count = 0;
	chunk->capacity = 0;
	chunk->code = NULL;
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
 * freeChunk - deletes the allocated dynamic array.
 * @chunk: pointer to a structure defining a dynamic array.
*/
void freeChunk(Chunk *chunk)
{
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	initChunk(chunk);
}
