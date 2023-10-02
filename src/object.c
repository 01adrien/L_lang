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
  return object;
}

object_t* allocate_string(char* str)
{
  value_t value = STRING_VAL(str);
  object_t* string = allocate_object(1, value);
  table_set(&vm.strings, AS_STRING(value), value);
  return string;
}

value_t get_string(char* string)
{
  value_t interned_str = table_get(&vm.strings, string);
  if (!IS_NIL(interned_str)) {
    FREE(string, char);
    return interned_str;
  }
  object_t* new_str = allocate_string(string);
  return new_str->value;
}

value_t concatenate_string(char* s2, char* s1)
{
  int len = strlen(s1) + strlen(s2) + 1;
  char* new = ALLOCATE(char, len);
  memcpy(new, s2, strlen(s2));
  memcpy(new + strlen(s2), s1, strlen(s1));
  new[len] = '\0';
  value_t str = get_string(new);
  return str;
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