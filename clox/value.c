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