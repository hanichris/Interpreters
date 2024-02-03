#ifndef clox_value_h
#define clox_value_h

#include "common.h"

typedef double Value;

/**
 * struct valAr - structure that wraps a pointer to an array
 * with its allocated capacity and number of elements in use.
 * @capacity: The allocated size of the array.
 * @count: The number of elements currently in the array.
 * @values: Pointer to an array whose size is unknown at compile time.
*/
typedef struct valAr{
    int capacity;
    int count;
    Value* values;
} ValueArray;


void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);

#endif