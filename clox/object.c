#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"


#define ALLOCATE_OBJ(type, objectType) \
	(type*)allocateObject(sizeof(type), objectType)

#define ALLOCATE_OBJ_VEC(type, length, objectType) \
	(type*)allocateObject(sizeof(type) + length * sizeof(char) + 1, objectType)

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
 * @hash: The hash code of the string literal.
*/
static ObjString* allocateString(char* chars, int length, uint32_t hash)
{
	ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
	string->length = length;
	string->hash = hash;
	string->chars = chars;
	return string;
}

static ObjStringVec* allocateStringVec(int length)
{
	ObjStringVec* string = ALLOCATE_OBJ_VEC(ObjStringVec, length, OBJ_STRING);
	string->length = length;
	tableSet(&vm.strings, string, NIL_VAL);
	return string;
}

/**
 * hashString - implementation of the `FNV-1a` hash function.
 * @key: key string to hash.
 * @length: length of the key string.
 * Return: The hash code of the key string.
*/
static uint32_t hashString(const char* key, int length)
{
	uint32_t hash = 2166136261u;
	for (size_t i = 0; i < length; i++)
	{
		hash ^= (uint32_t)key[i];
		hash *= 16777619;
	}
	return hash;
}

ObjStringVec* takeStringVec(ObjStringVec* a, ObjStringVec* b)
{
	int length = a->length + b->length;
	ObjStringVec* string = allocateStringVec(length);
	memcpy(string->chars, a->chars, a->length);
	memcpy(string->chars + a->length, b->chars, b->length);
	string->chars[length] = '\0';
	uint32_t hash = hashString(string->chars, length);
	string->hash = hash;

	return string;
}

ObjStringVec* copyStringVec(const char* chars, int length)
{
	uint32_t hash = hashString(chars, length);
	
	ObjStringVec* string = allocateStringVec(length);
	memcpy(string->chars, chars, length);
	string->chars[length] = '\0';
	return string;
}

ObjString* takeString(char* chars, int length)
{
	uint32_t hash = hashString(chars, length);
	return allocateString(chars, length, hash);
}

ObjString* copyString(const char* chars, int length)
{
	uint32_t hash = hashString(chars, length);
	char* heapChars = ALLOCATE(char, length + 1);
	memcpy(heapChars, chars, length);
	heapChars[length] = '\0';
	return allocateString(heapChars, length, hash);
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