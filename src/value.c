#include "includes/value.h"
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
}