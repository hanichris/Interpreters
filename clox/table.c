#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"


#define TABLE_MAX_LOAD 0.75

/**
 * initTable - Initializes a new empty hash table.
 * @table: pointer to a hash table struct type.
 * Return: void.
*/
void initTable(Table* table)
{
	table->count = 0;
	table->capacity = 0;
	table->entries = NULL;
}

/**
 * freeTable - Frees a previously allocated hash table array.
 * @table: pointer to an allocated hash table.
 * Return: void.
*/
void freeTable(Table* table)
{
	FREE_ARRAY(Entry, table->entries, table->capacity);
	initTable(table);
}

static Entry* findEntry(Entry* entries, int capacity, ObjStringVec* key)
{
	uint32_t index = key->hash % capacity;
	for (;;)
	{
		Entry* entry = &entries[index];
		if (entry->key == key || entry->key == NULL)
		{
			return entry;
		}
		index = (index + 1) % capacity;
	}
}

/**
 * tableSet - insert a value into the hash table.
 * @table: pointer to the hash table.
 * @key: key string for the value.
 * @value: The value to be added to the hash table.
 * Return: if the value added was new.
*/
bool tableSet(Table* table, ObjStringVec* key, Value value)
{
	if (table->count + 1 > table->capacity * TABLE_MAX_LOAD)
	{
		int capacity = GROW_CAPACITY(table->capacity);
		adjustCapacity(table, capacity);
	}
	
	Entry* entry = findEntry(table->entries, table->capacity, key);
	bool isNewKey = entry->key == NULL;
	if (isNewKey) table->count++;

	entry->key = key;
	entry->value = value;
	return isNewKey;
	
}