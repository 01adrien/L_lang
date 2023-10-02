#ifndef _TABLE_H_
#define _TABLE_H_

#include "common.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

typedef struct entry {
  const char* key;
  value_t value;
} entry_t;

typedef struct table {
  entry_t* entries;
  int capacity;
  int count;
} table_t;

value_t table_get(table_t* table, const char* key);
bool table_delete(table_t* table, const char* key);
void table_init(table_t* table);
void table_free(table_t* table);
bool table_set(table_t* table, const char* key, value_t value);

#endif