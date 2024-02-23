#include <stdlib.h>

#include "memory.h"
#include "value.h"

/**
 * initValueArray - initialize the dynamic array data structure
 * related to the 'Value' datatype.
 * @array: pointer to the array along with its allocated capacity
 * and number of elements in use.
 * Return: void.
*/
void initValueArray(ValueArray* array)
{
	array->count = 0;
	array->capacity = 0;
	array->values = NULL;
}

/**
 * writeValueArray - Add a value to the dynamic array by making use of
 * the memory-management macros.
 * @array: pointer to the dynamic array.
 * @value: value to be added to the array.
 * Return: void.
*/
void writeValueArray(ValueArray* array, Value value)
{
	if (array->capacity < array->count + 1)
	{
		int oldCapacity = array->capacity;
		array->capacity = GROW_CAPACITY(oldCapacity);
		array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
	}
	
	array->values[array->count] = value;
	array->count++;
}

/**
 * freeValueArray - Releases all the memory used up by the array.
 * @array: pointer to the dynamic array.
 * Return: void.
*/
void freeValueArray(ValueArray* array)
{
	FREE_ARRAY(Value, array->values, array->capacity);
	initValueArray(array);
}

/**
 * printValue - print out the value passed to the function using the '%g'
 * specifier.
 * @value: value to print out to the console.
 * Return: void.
*/
void printValue(Value value)
{
	switch (value.type)
	{
	case VAL_BOOL:
		printf(AS_BOOL(value) ? "true" : "false");
		break;
	case VAL_NIL: printf("nil"); break;
	case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
	}
}

/**
 * valuesEqual - determines if two `Value` objects are equal.
 * @a: fist value to test with.
 * @b: second value to test against.
 * Return: true or false.
*/
bool valuesEqual(Value a, Value b)
{
	if (a.type != b.type) return false;

	switch (a.type)
	{
		case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
		case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
		case VAL_NIL: return true;
		default: return false;
	}
}