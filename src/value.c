#include "includes/value.h"
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
value_type_t type_of(value_t value)
{
  if (!isnan(value.as_double)) {
    return NUMBER_T;
  }
  uint16_t type = (*(uint64_t*) &value.as_int) >> 48;
  switch (type) {
  case STRING_T:
    return STRING_T;
    break;
  case BOOLEAN_T:
    return BOOLEAN_T;
    break;
  case NIL_T:
    return NIL_T;
  default:
    printf("unknow type\n");
    break;
  }
}