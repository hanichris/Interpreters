#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, char **argv)
{
	Chunk chunk;

	initChunk(&chunk);
	int constant = addConstant(&chunk, 1.2);
	writeChunk(&chunk, OP_CONSTANT);
	writeChunk(&chunk, constant);
	writeChunk(&chunk, OP_RETURN);
	/* pass the chunk to the disassembler. */
	disassembleChunk(&chunk, "Test chunk");
	freeChunk(&chunk);
	return (0);
}
