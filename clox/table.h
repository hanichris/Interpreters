#if !defined(clox_table_h)
#define clox_table_h

#include "common.h"
#include "value.h"

/**
 * struct _entry - Defines a simple key/value pair. The key is
 * always a string which is stored directly as a pointer to `ObjString`
 * making it a faster and smaller implementation.
 * @key: Key string value.
 * @value: Value being stored at the provided key.
*/
typedef struct _entry
{
	ObjStringVec* key;
	Value value;
} Entry;


/**
 * struct _table - A Hash table defined as a
 * dynamic array of entries. Keep track of the
 * allocated size `capacity` and the `count` of
 * key/value pairs currently stored. The ratio of
 * `count` to `capacity` is the `load factor`.
 * @count: The number of key/value pairs in the hash table.
 * @capacity: The allocated size of the hash table.
 * @entries: Pointer to the array of entries.
*/
typedef struct _table
{
    int count;
	int capacity;
	Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);
bool tableSet(Table* table, ObjStringVec* key, Value value);

#endif // clox_table_h
