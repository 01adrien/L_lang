#include "includes/value.h"
#include "includes/object.h"
#include <math.h>
#include <stdio.h>

void print_value(value_t value)
{
  if (IS_NUMBER(value)) {
    printf("%.f", AS_NUMBER(value));
  }
  if (IS_BOOLEAN(value)) {
    printf("%s", AS_BOOLEAN(value) == 1 ? "true" : "false");
  }
  if (IS_NIL(value)) {
    printf("nil");
  }
  if (IS_STRING(value)) {
    printf("%s", AS_STRING(value));
  }
}

bool values_equal(value_t v1, value_t v2)
{
  if (type_of(v1) != type_of(v2)) {
    return false;
  }
  switch (type_of(v1)) {
  case NUMBER_T:
    return AS_NUMBER(v1) == AS_NUMBER(v2);
  case NIL_T:
    return true;
  case BOOLEAN_T:
    return AS_BOOLEAN(v1) == AS_BOOLEAN(v2);
  case STRING_T:
    return strcmp(AS_STRING(v1), AS_STRING(v2)) == 0;
  default:
    return false;
  }
}

value_type_t type_of(value_t value)
{
  if (!isnan(value.as_double)) {
    return NUMBER_T;
  }
  return (*(uint64_t*) &value.as_int) >> 48;
}