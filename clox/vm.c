#include "debug.h"
#include "compiler.h"
#include "vm.h"

VM vm;

static InterpretResult run()
{
	#define READ_BYTE() (*vm.ip++)
	#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
	#define BINARY_OP(op) \
			do { \
				double b = pop(); \
				double a = pop(); \
				push(a op b); \
			} while (false)
			

	for (;;)
	{
		#if defined(DEBUG_TRACE_EXECUTION)
		printf("          ");
		for (Value* slot = vm.stack; slot < vm.stackTop; slot++)
		{
			printf("[ ");
			printValue(*slot);
			printf(" ]");
		}
		printf("\n");
		disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));

		#endif // DEBUG_TRACE_EXECUTION
		
		u_int8_t instruction;

		switch (instruction = READ_BYTE())
		{
			case OP_CONSTANT: {
				Value constant = READ_CONSTANT();
				push(constant);
				break;
			}

			case OP_ADD: BINARY_OP(+); break;
			case OP_SUBTRACT: BINARY_OP(-); break;
			case OP_MULTIPLY: BINARY_OP(*); break;
			case OP_DIVIDE: BINARY_OP(/); break;

			case OP_NEGATE: {
				// push(-pop()); break;
				*(vm.stack + (int)(vm.stackTop - vm.stack) - 1) =
					-*(vm.stack + (int)(vm.stackTop - vm.stack) - 1);
				break;
			}

			case OP_RETURN: {
				printValue(pop());
				printf("\n");
				return INTERPRET_OK;
			}
		}
	}

	#undef BINARY_OP
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

/**
 * interpret - Fills us a chunk with bytecode generated from the
 * user's program and executes the chunk of bytecode if no
 * compilation errors were encountered.
 * @source: user's source program to execute.
 * Return: INTERPRET_COMPILE_ERROR | INTERPRET_RUNTIME_ERROR | INTERPRET_OK
*/
InterpretResult interpret(const char* source)
{
	Chunk chunk;
	initChunk(&chunk);
	if (!compile(source, &chunk))
	{
		freeChunk(&chunk);
		return INTERPRET_COMPILE_ERROR;
	}

	vm.chunk = &chunk;
	vm.ip = vm.chunk->code;

	InterpretResult result = run();

	freeChunk(&chunk);

	return result;
}