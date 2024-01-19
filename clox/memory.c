#include <stdlib.h>

#include "memory.h"

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
