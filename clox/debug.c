#include <stdio.h>

#include "debug.h"
#include "value.h"

/**
 * simpleInstruction - simple utility function that displays an instruction.
 * @name: Instrucntion name.
 * @offset: current position in the bytecode.
 * Return: The position of the next instruction in the chunk.
*/
static int simpleInstruction(const char *name, int offset)
{
	printf("%s\n", name);
	return (offset + 1);
}

/**
 * constantInstruction - Pulls out the constant index from the subsequent
 * byte in the chunk and prints out the name of the opcode, the index and
 * the value corresponding to the index since it is known at compile time.
 * @name: Name of the opcode.
 * @chunk: pointer to the dynamic array defining a chunk of bytecode.
 * @offset: current position of the instruction in the bytecode chunk.
 * Return: The position of the next instruction in the chunk.
*/
static int constantInstruction(const char *name, Chunk* chunk, int offset)
{
	u_int8_t constant = chunk->code[offset + 1];
	printf("%-16s %4d '", name, constant);
	printValue(chunk->constants.values[constant]);
	printf("'\n");
	return offset + 2;
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
 * Return: The position of the next instruction in the chunk.
*/
int disassembleInstruction(Chunk *chunk, int offset)
{
	u_int8_t instruction = chunk->code[offset];

	printf("%04d ", offset);
	if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1])
	{
		printf("   | ");
	} else
	{
		printf("%4d ", chunk->lines[offset]);
	}

	switch (instruction)
	{
		case OP_CONSTANT:
			return constantInstruction("OP_CONSTANT", chunk, offset);

		case OP_ADD:
			return simpleInstruction("OP_ADD", offset);

		case OP_SUBTRACT:
			return simpleInstruction("OP_SUBTRACT", offset);

		case OP_MULTIPLY:
			return simpleInstruction("OP_MULTIPLY", offset);

		case OP_DIVIDE:
			return simpleInstruction("OP_DIVIDE", offset);

		case OP_NEGATE:
			return simpleInstruction("OP_NEGATE", offset);

		case OP_RETURN:
			/* code */
			return simpleInstruction("OP_RETURN", offset);

		default:
			printf("Unknown opcode %d\n", instruction);
			return offset + 1;
	}
}
