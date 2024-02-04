#if !defined(clox_vm_h)
#define clox_vm_h

#include "chunk.h"

/**
 * struct vm - structure to hold the vm's internal state.
 * @chunk: pointer to the chunk the vm executes.
 * @ip: pointer to the location of the currently executing instruction.
*/
typedef struct virtualMachine
{
	Chunk* chunk;
	u_int8_t* ip;
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

void initVM();
void freeVM();
InterpretResult interpret(Chunk* chunk);

#endif // clox_vm_h
