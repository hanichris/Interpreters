#if !defined(clox_object_h)
#define clox_object_h

#include "common.h"
#include "value.h"

#define OBJ_TYPE(value)			(AS_OBJ(value)->type)
#define IS_STRING(value)		isObjType(value, OBJ_STRING)

#define AS_STRING(value)		((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)		(((ObjString*)AS_OBJ(value))->chars)

/**
 * enum _obj_type - defines the supported object types.
*/
typedef enum _obj_type
{
	OBJ_STRING,
} ObjType;

/**
 * struct Obj - contains the state shared across all object
 * types. Acts like a 'base class' for objects.
 * @type: the `type` tag of the object.
 * @next: pointer to the next `Obj` in the chain.
*/
struct Obj
{
	ObjType type;
	struct Obj* next;
};

/**
 * struct ObjString - defines the payload for `string` object types.
 * It contains an array of characters stored in a separate heap-allocated
 * array, the number of bytes in the array and the state shared by all objects.
 * @obj: common state shared by all `object` types.
 * @length: number of bytes in the array of characters (the length of the array).
 * @chars: pointer to the heap-allocated array.
*/
struct ObjString
{
	Obj obj;
	int length;
	char* chars;
};

static inline bool isObjType(Value value, ObjType type)
{
	return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);
void printObject(Value value);


#endif // clox_object_h
