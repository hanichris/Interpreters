#include <string.h>
#include <stdarg.h>

#include "debug.h"
#include "compiler.h"
#include "memory.h"
#include "vm.h"

VM vm;

/**
 * peek - Gets a `Value` from the stack but does not pop it.
 * @distance: How far down from the top of the stack to look - zero is
 * the top, one is one slot down etc.
 * Return: The `Value` at the given distance from the top.
*/
static Value peek(int distance)
{
	return vm.stackTop[-1 - distance];
}

static bool isFalsey(Value value)
{
	return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

/**
 * concatentate - joins together two string literals. Starts by calculating
 * the length of the resultant string from the lengths of the two operands.
 * Allocates a character array for the result and copies the two halves in.
 * It finally properly terminates the string.
*/
static void concatenate()
{
	ObjStringVec* b = AS_STRING(pop());
	ObjStringVec* a = AS_STRING(pop());

	ObjStringVec* result = takeStringVec(a, b);
	push(OBJ_VAL(result));
}

/**
 * resetStack - resets the stack by setting the pointer `stackTop`
 * to point to the beginning of the array signifying an empty stack.
 * Ensures there are no allocated objects.
*/
static void resetStack()
{
	memset(vm.stack, 0, 256 * sizeof(Value));
	vm.stackTop = vm.stack;
	vm.objects = NULL;
}

/**
 * runtimeError - reports a useful error message to the user with the line
 * of their code that was being executed when the error occurred.
 * @format: the message string with a format layout.
*/
static void runtimeError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

	size_t instruction = vm.ip - vm.chunk->code - 1;
	int line = vm.chunk->lines[instruction];
	fprintf(stderr, "[line %d] in script\n", line);
	resetStack();

}

static InterpretResult run()
{
	#define READ_BYTE() (*vm.ip++)
	#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
	#define READ_STRING() AS_STRING(READ_CONSTANT())
	#define BINARY_OP(valueType, op) \
			do { \
				if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
					runtimeError("Operands must be numbers."); \
					return INTERPRET_RUNTIME_ERROR; \
				} \
				double b = AS_NUMBER(pop()); \
				double a = AS_NUMBER(pop()); \
				push(valueType(a op b)); \
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

			case OP_FALSE: push(BOOL_VAL(false)); break;
			case OP_TRUE: push(BOOL_VAL(true)); break;
			case OP_NIL: push(NIL_VAL); break;

			case OP_EQUAL: {
				Value b = pop();
				Value a = pop();
				push(BOOL_VAL(valuesEqual(a, b)));
				break;
			}
			case OP_GREATER:	BINARY_OP(BOOL_VAL, >); break;
			case OP_LESS:		BINARY_OP(BOOL_VAL, <); break;

			case OP_ADD: {
				if (IS_STRING(peek(0)) && IS_STRING(peek(1)))
				{
					concatenate();
				} else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1)))
				{
					double a = AS_NUMBER(pop());
					double b = AS_NUMBER(pop());
					push(NUMBER_VAL(a + b));
				} else
				{
					
					runtimeError("Operands must be two numbers or two strings");
					return INTERPRET_RUNTIME_ERROR;
				}
				break;
			}
			case OP_SUBTRACT: 	BINARY_OP(NUMBER_VAL, -); break;
			case OP_MULTIPLY: 	BINARY_OP(NUMBER_VAL, *); break;
			case OP_DIVIDE: 	BINARY_OP(NUMBER_VAL, /); break;

			case OP_NOT: push(BOOL_VAL(isFalsey(pop()))); break;

			case OP_NEGATE: {
				if (!IS_NUMBER(peek(0)))
				{
					runtimeError("Operand must be a number");
					return INTERPRET_RUNTIME_ERROR;
				}

				// push(-pop()); break;
				*(vm.stack + (int)(vm.stackTop - vm.stack) - 1) =
					NUMBER_VAL(-AS_NUMBER(*(vm.stack + (int)(vm.stackTop - vm.stack) - 1)));
				break;
			}

			case OP_POP: pop(); break;
			case OP_DEFINE_GLOBAL: {
				ObjStringVec* name = READ_STRING();
				tableSet(&vm.globals, name, peek(0));
				pop();
				break;
			}

			case OP_PRINT: {
				printValue(pop());
				printf("\n");
				break;
			}

			case OP_RETURN: {
				return INTERPRET_OK;
			}
		}
	}

	#undef BINARY_OP
	#undef READ_CONSTANT
	#undef READ_STRING
	#undef READ_BYTE

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
	vm.objects = NULL;
	initTable(&vm.strings);
	initTable(&vm.globals);
}

void freeVM()
{
	freeTable(&vm.strings);
	freeTable(&vm.globals);
	freeObjects();
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
	if (vm.stackTop == vm.stack)
	{
		fprintf(stderr, "Trying to pop from an empty stack\n");
		return NIL_VAL;
	}
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