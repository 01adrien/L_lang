#include "includes/table.h"
#include "includes/memory.h"
#include "includes/value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

entry_t* table_find(entry_t* entries, int capacity, const char* key);
void table_adjust_capacity(table_t* table, int capacity);
uint32_t hash_string(const char* key, int length);

void table_init(table_t* table)
{
  table->count = 0;
  table->capacity = 10;
  table->entries = ALLOCATE(entry_t, table->capacity);
  for (int i = 0; i < table->capacity; i++) {
    table->entries[i].key = NULL;
    table->entries[i].value = NIL_VAL;
  }
}

void table_free(table_t* table)
{
  FREE_ARRAY(entry_t, table->entries, table->capacity);
  // init_table(table);
}

value_t table_get(table_t* table, const char* key)
{
  uint32_t hash = hash_string(key, strlen(key));
  size_t index = (size_t) (hash & (uint64_t) (table->capacity - 1));
  entry_t* entries = table->entries;

  while (entries[index].key != NULL) {
    if ((strcmp(key, entries[index].key) == 0)) {
      return entries[index].value;
    }
    index++;
    if (index >= table->capacity) {
      index = 0;
    }
  }
  return NIL_VAL;
}

bool table_set(table_t* table, const char* key, value_t value)
{
  entry_t* entries = table->entries;
  uint32_t hash = hash_string(key, strlen(key));
  size_t index = (size_t) (hash & (uint64_t) (table->capacity - 1));
  while (entries[index].key != NULL) {
    // if (strcmp(key, entries[index].key) == 0) {
    //   table->entries[index].value = value;
    //   return true;
    // }
    index++;
    if (index >= table->capacity) {
      index = 0;
    }
  }
  entries[index].key = key;
  entries[index].value = value;
  return true;
}

bool table_delete(table_t* table, const char* key)
{
}

entry_t* table_find(entry_t* entries, int capacity, const char* key)
{
}

void table_adjust_capacity(table_t* table, int capacity)
{
}

uint32_t hash_string(const char* key, int length)
{
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= key[i];
    hash *= 16777619;
  }
  return hash;
}