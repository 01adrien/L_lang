#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "value.h"

typedef struct object {
  value_t value;
  struct object* next;
} object_t;

void free_objects();
object_t* allocate_string(const char* str, int len);
object_t* allocate_object(size_t size, value_t value);

#endif   // !