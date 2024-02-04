#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char **argv)
{
	Chunk chunk;
	
	initVM();

	initChunk(&chunk);
	int constant = addConstant(&chunk, 1.2);
	writeChunk(&chunk, OP_CONSTANT, 123);
	writeChunk(&chunk, constant, 123);
	writeChunk(&chunk, OP_RETURN, 123);
	/* pass the chunk to the disassembler. */
	// disassembleChunk(&chunk, "Test chunk");
	/* command the VM to interpret a chunk of bytecode. */
	interpret(&chunk);
	freeVM();
	freeChunk(&chunk);
	return (0);
}
