#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"


#define ALLOCATE_OBJ(type, objectType) \
	(type*)allocateObject(sizeof(type), objectType)

/**
 * allocateObject - allocates an object of the given size on the heap. Size
 * isn't just the size of the `Obj` but also for the extra payload fields
 * needed by a specific object type being created.
 * @size: The overall size of the object type being created.
 * @type: type of object being created.
*/
static Obj* allocateObject(size_t size, ObjType type)
{
	Obj* object = (Obj*)reallocate(NULL, 0, size);
	object->type = type;
	object->next = vm.objects;
	vm.objects = object;
	return object;
}

/**
 * allocateString - creates a string object. It creates a new ObjString
 * on the heap and initializes its fields. Acts like a constructor in OOP.
 * First calls the `base class` constructor to initialize the `Obj` state.
 * @chars: array of characters to be converted to an object of type ObjString.
 * @length: The length of the array of characters.
*/
static ObjString* allocateString(char* chars, int length)
{
	ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
	string->length = length;
	string->chars = chars;
	return string;
}

ObjString* takeString(char* chars, int length)
{
	return allocateString(chars, length);
}

ObjString* copyString(const char* chars, int length)
{
	char* heapChars = ALLOCATE(char, length + 1);
	memcpy(heapChars, chars, length);
	heapChars[length] = '\0';
	return allocateString(heapChars, length);
}

void printObject(Value value)
{
	switch (OBJ_TYPE(value))
	{
		case OBJ_STRING:
			printf("%s", AS_CSTRING(value));
			break;
		
		default:
			break;
	}
}