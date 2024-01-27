#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, char **argv)
{
	Chunk chunk;

	initChunk(&chunk);
	writeChunk(&chunk, OP_RETURN);
	/* pass the chunk to the disassembler. */
	disassembleChunk(&chunk, "Test chunk");
	freeChunk(&chunk);
	return (0);
}
