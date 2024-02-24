#ifndef clox_value_h
#define clox_value_h

#include "common.h"

typedef struct Obj Obj;

/**
 * enum _value_type - Describes a type "tag" for each of the
 * type possibilities.
 * @VAL_BOOL: type "tag" for boolean types.
 * @VAL_NIL: type "tag" for nil types.
 * @VAL_NUMBER: type "tag" for number types.
 * @VAL_OBJ: type "tag" for object types.
*/
typedef enum _value_type
{
	VAL_BOOL,
	VAL_NIL,
	VAL_NUMBER,
	VAL_OBJ
} ValueType;

/**
 * struct _value - Describes a "tagged union" that is a value with
 * two parts: a type "tag", and a payload for the actual value.
 * @type: the type "tag" for the value in question.
 * @as: the payload for the value.
*/
typedef struct _value
{
	ValueType type;
	union
	{
		bool boolean;
		double number;
		Obj* obj;
	} as;
} Value;

/**
 * Make provision for error checking to ensure safe use of the `As_` macros.
*/

#define IS_BOOL(value)		((value).type == VAL_BOOL)
#define IS_NIL(value)		((value).type == VAL_NIL)
#define IS_NUMBER(value)	((value).type == VAL_NUMBER)
#define IS_OBJ(value)		((value).type == VAL_OBJ)

/**
 * Unpacks a clox Value to get the underlying C value.
 * Given a Value of the right type, unwrap it and return the
 * corresponding raw C value.
*/

#define AS_BOOL(value) 	 ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJ(value)	 ((value).as.obj)
/**
 * Promote a native C value to a clox Value. Each one of these takes a
 * C value of the appropriate type and produces a Value with the correct
 * type tag and contains the underlying value. This hoists statically
 * typed values up into clox's dynamically typed universe.
*/

#define BOOL_VAL(value)   ((Value){ VAL_BOOL, .as.boolean = value })
#define NIL_VAL			  ((Value){ VAL_NIL, .as.number = 0 })
#define NUMBER_VAL(value) ((Value){ VAL_NUMBER, { .number = value } })
#define OBJ_VAL(object)	  ((Value){ VAL_OBJ, { .obj = (Obj*)object }})

/**
 * struct valAr - structure that wraps a pointer to an array
 * with its allocated capacity and number of elements in use.
 * @capacity: The allocated size of the array.
 * @count: The number of elements currently in the array.
 * @values: Pointer to an array whose size is unknown at compile time
 * and which will hold the literal values of constants.
*/
typedef struct valAr
{
	int capacity;
	int count;
	Value* values;
} ValueArray;

bool valuesEqual(Value a, Value b);
void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);

#endif