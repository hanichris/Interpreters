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

/**
 * findEntry - takes a key and an array of buckets and determines which
 * bucket an entry belongs to. It also performs linear probing and collision
 * handling.
 * @entries: the array of buckets holding different key/value pairs.
 * @capacity: the size of the allocated array of entries.
 * @key: the key being looked for in the array of entries.
*/
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
 * tableGet - Given a key retrieves the value from the hash table.
 * Detects whether the array of buckets is empty and returns false.
 * If the entry pointed to by the provided key is null, also returns false.
 * Finally, if the key points to a non-empty entry, the value is assigned
 * to the output parameter `value` and the function returns true.
 * @table: the hash table to search through.
 * @key: the value's key.
 * @value: the output parameter to hold the obtained value.
 * Return: boolean value of whether a value was found.
*/
bool tableGet(Table* table, ObjStringVec* key, Value* value)
{
	if (table->count == 0) return false;

	Entry* entry = findEntry(table->entries, table->capacity, key);
	if (entry->key == NULL) return false;

	*value = entry->value;
	return false;
}

/**
 * adjustCapacity - allocates an array of buckets which are initialized to
 * `NIL_VAL` with `NULL` key strings. It then copies over the non-empty buckets
 * from the older hash table array to the newly created one before freeing the
 * memory that the old array occupied.
 * @table: pointer to the hash table.
 * @capacity: the size of the hash table.
 * Return: void.
*/
static void adjustCapacity(Table* table, int capacity)
{
	Entry* entries = ALLOCATE(Entry, capacity);
	for (size_t i = 0; i < capacity; i++)
	{
		entries[i].key = NULL;
		entries[i].value = NIL_VAL;
	}

	for (size_t i = 0; i < table->capacity; i++)
	{
		Entry* entry = &table->entries[i];
		if (entry->key == NULL) continue;

		Entry* dest = findEntry(entries, capacity, entry->key);
		dest->key = entry->key;
		dest->value = entry->value;
	}
	
	FREE_ARRAY(Entry, table->entries, table->capacity);

	table->entries = entries;
	table->capacity = capacity;
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

/**
 * tableAddAll - Walks the bucket array of the source hash table
 * and adds any non-empty entries found to the destination hash table
 * using the `tableSet` function.
*/
void tableAddAll(Table* from, Table* to)
{
	for (size_t i = 0; i < from->capacity; i++)
	{
		Entry* entry = &from->entries[i];
		if (entry->key != NULL)
		{
			tableSet(to, entry->key, entry->value);
		}
	}
}