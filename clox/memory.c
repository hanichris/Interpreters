#include <stdlib.h>

#include "memory.h"
#include "vm.h"

/**
 * reallocate - performs the needed dynamic memory management.
 * This entails allocating memory, freeing it and changing the size
 * of an existing allocation. The two size arguments control which
 * operation of the 3 is to be performed.
 * Allocate a new block when `oldSize` == 0 && `newSize` > 0.
 * Free an allocation when `oldSize` > 0 && `newSize` == 0.
 * Shrink an allocation when 0 < `newSize` < `oldSize`.
 * Grow an allocation when 0 < `oldSize` < `newSize`.
 * @pointer: pointer to memory to manage.
 * @oldSize: old size of memory pointed to by pointer.
 * @newSize: new desired size of memory pointed to by pointer.
 * Return: void pointer.
*/
void *reallocate(void *pointer, size_t oldSize, size_t newSize)
{
	void *result = NULL;

	if (newSize == 0)
	{
		free(pointer);
		return (NULL);
	}

	result = realloc(pointer, newSize);
	if (result == NULL)
		exit(EXIT_FAILURE);
	return (result);
}

/**
 * freeObject - frees the memory that an object type owns before
 * freeing the object itself.
 * object - pointer to the object to be freed.
*/
static void freeObject(Obj* object)
{
	switch (object->type)
	{
		case OBJ_STRING: {
			ObjString* string = (ObjString*)object;
			FREE_ARRAY(char, string->chars, string->length + 1);
			FREE(ObjString, object);
			break;
		}

		default:
			break;
	}
}

/**
 * freeObjects - walks the linked list and frees its nodes.
*/
void freeObjects()
{
	Obj* object = vm.objects;
	while (object != NULL)
	{
		Obj* next = object->next;
		freeObject(object);
		object = next;
	}
	
}