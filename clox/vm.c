#include "vm.h"

VM vm;

static InterpretResult run()
{
	#define READ_BYTE() (*vm.ip++)
	#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

	for (;;)
	{
		u_int8_t instruction;

		switch (instruction = READ_BYTE())
		{
			case OP_CONSTANT: {
				Value value = READ_CONSTANT();
				printValue(value);
				printf("\n");
				break;
			}

			case OP_RETURN:
				return INTERPRET_OK;
		}
	}

	#undef READ_CONSTANT
	#undef READ_BYTE
	
}

void initVM()
{

}

void freeVM()
{

}

InterpretResult interpret(Chunk* chunk)
{
	vm.chunk = chunk;
	vm.ip = vm.chunk->code;
	return run();
}