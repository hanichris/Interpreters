#include "debug.h"
#include "vm.h"

VM vm;

static InterpretResult run()
{
	#define READ_BYTE() (*vm.ip++)
	#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

	for (;;)
	{
		#if defined(DEBUG_TRACE_EXECUTION)
		disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));

		#endif // DEBUG_TRACE_EXECUTION
		
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

/**
 * resetStack - resets the stack by setting the pointer `stackTop`
 * to point to the beginning of the array signifying an empty stack.
*/
static void resetStack()
{
	vm.stackTop = vm.stack;
}

/**
 * initVM - initializes the internal state of the VM by setting the pointer
 * of the top of the stack to the beginning of the stack array. There is no
 * need to allocate space for the array as it is declared inline within the VM
 * struct. Moreover, there is no need to clear unused cells as they simply won't
 * be accessed until after values are stored within them.
*/
void initVM()
{
	resetStack();
}

void freeVM()
{

}

/**
 * push - pushes a new value on o the top of the stack. After which,
 * it increments the `stackTop` pointer to point to the next unused
 * slot in the array since the previous one now holds a value.
 * @value: value to be placed onto the top of the stack.
 * Return: void.
*/
void push(Value value)
{
	*vm.stackTop = value;
	vm.stackTop++;
}

/**
 * pop - retrieves the most recenty pushed value from the top of the stack.
 * It first decrements the `stackTop` pointer to the value at the top of the
 * stack before retrieving said value.
 * Return: the value at the top of the stack.
*/
Value pop()
{
	vm.stackTop--;
	return *vm.stackTop;
}

InterpretResult interpret(Chunk* chunk)
{
	vm.chunk = chunk;
	vm.ip = vm.chunk->code;
	return run();
}