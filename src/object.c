#include "includes/object.h"
#include "includes/memory.h"
#include "includes/table.h"
#include "includes/value.h"
#include "includes/vm.h"
#include <stdio.h>

void free_object(object_t* object);

object_t* allocate_object(size_t size, value_t value)
{
  object_t* object = ALLOCATE(object_t, 1);
  object->value = value;
  object->next = vm.objects;
  vm.objects = object;
  // if (type_of(value) == STRING_T) {
  //   table_set(&vm.strings, AS_STRING(value), NIL_VAL);
  // }
  return object;
}

object_t* allocate_string(const char* str, int len)
{
  char* chars = calloc(len, sizeof(char));
  memcpy(chars, str, len);
  value_t value = STRING_VAL(chars);
  object_t* string = allocate_object(1, value);
  return string;
}

void free_objects()
{
  object_t* temp = vm.objects;
  while (temp) {
    free_object(temp);
    object_t* del = temp;
    temp = temp->next;
    FREE(del, object_t);
  }
}

void free_object(object_t* object)
{
  value_type_t type = type_of(object->value);
  switch (type) {
  case STRING_T:
    FREE(AS_STRING(object->value), char);
    break;
  default:
    break;
  }
}