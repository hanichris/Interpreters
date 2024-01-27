#include <stdio.h>

#include "debug.h"

/**
 * simpleInstruction - simple utility function that displays an instruction.
 * @name: Instrucntion name.
 * @offset: current position in the bytecode.
 * Return: The next offset position.
*/
static int simpleInstruction(const char *name, int offset)
{
	printf("%s\n", name);
	return (offset + 1);
}

/**
 * disassembleChunk - disassembles all the instructions in the
 * entire chunk. It is implemented in terms of another function.
 * @chunk: dynamic array pointer holding the instructions.
 * @name: header to show which chunk is being examined.
 * Return: void.
*/
void disassembleChunk(Chunk *chunk, const char *name)
{
	printf("== %s ==\n", name);

	for (int offset = 0; offset < chunk->count;)
		offset = disassembleInstruction(chunk, offset);
}

/**
 * disassembleInstruction - core of the `debug` module. It starts by first
 * printing the byte offset of the given instruction to mark where in the
 * chunk the instruction lies. It then reads the a single byte from the
 * bytecode at the given offset and switch on it. For each instruction,
 * dispatch a utility function to display it.
 * @chunk: dynamic array representing a chunk of bytecode.
 * @offset: position on the dynamic array currently being examined.
 * Return: The next offset position in the chunk.
*/
int disassembleInstruction(Chunk *chunk, int offset)
{
	u_int8_t instruction = chunk->code[offset];

	printf("%04d ", offset);
	switch (instruction)
	{
		case (OP_RETURN):
			/* code */
			return (simpleInstruction("OP_RETURN", offset));

		default:
			printf("Unknown opcode %d\n", instruction);
			return (offset + 1);
	}
}
