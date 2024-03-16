#if !defined(clox_vm_h)
#define clox_vm_h

#include "chunk.h"
#include "table.h"

#define STACK_MAX 256

/**
 * struct vm - structure to hold the vm's internal state.
 * @chunk: pointer to the chunk the vm executes.
 * @ip: pointer to the location of the currently executing instruction.
 * @stack: keeps track of the temporary values generated by an expression.
 * @stackTop: pointer to the top of the stack where the next value will
 * be written to.
 * @objects: pointer to the head of an intrusive list that keeps track of
 * the heap-allocated `Objs`.
*/
typedef struct virtualMachine
{
	Chunk* chunk;
	u_int8_t* ip;
	Value stack[STACK_MAX];
	Value* stackTop;
	Table strings;
	Obj* objects;
} VM;

/**
 * enum status - enumerates the possible outcomes of for the
 * virtual machine.
 * @INTERPRET_OK: no issues occurred.
 * @INTREPRET_COMPILE_ERROR: a compile time error occurred.
 * @INTREPRET_RUNTIME_ERROR: a runtime error occurred.
*/
typedef enum status
{
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR
} InterpretResult;

extern VM vm;

void initVM();
void freeVM();
InterpretResult interpret(const char* source);
void push(Value value); // stack protocol supports these two operations.
Value pop();

#endif // clox_vm_h
