#ifndef _VALUE_H_
#define _VALUE_H_

#include "common.h"

typedef union value {
  double as_double;
  uint64_t as_int;
} value_t;

typedef enum value_type {
  STRING_T = 0x7ffd,
  BOOLEAN_T = 0x7ffe,
  NIL_T = 0xffff,
  NUMBER_T,
} value_type_t;

#define _NAN_        0xffff000000000000
#define OUR_NAN(v)   ((v.as_int & 0x7ffc000000000000) == 0x7ffc000000000000)
#define NAN_VALUE(v) (v.as_int & (~_NAN_))

#define BOOLEAN 0x7ffe000000000002
#define STRING  0x7ffd000000000000
#define NIL     0xffff000000000000

#define IS_STRING(v)  ((v.as_int & _NAN_) == STRING)
#define IS_NUMBER(v)  (!OUR_NAN(v))
#define IS_NIL(v)     (v.as_int == 0xffff000000000000)
#define IS_BOOLEAN(v) ((v.as_int & BOOLEAN) == BOOLEAN)

#define AS_STRING(v)  ((char*) NAN_VALUE(v))
#define AS_NUMBER(v)  (v.as_double)
#define AS_BOOLEAN(v) ((int8_t) (v.as_int & 0x1))

#define STRING_VAL(p)                                                          \
  (value_t)                                                                    \
  {                                                                            \
    .as_int = (uint64_t) (p) | STRING                                          \
  }

#define NUMBER_VAL(d)                                                          \
  (value_t)                                                                    \
  {                                                                            \
    .as_double = d                                                             \
  }

#define TRUE_VAL                                                               \
  (value_t)                                                                    \
  {                                                                            \
    .as_int = BOOLEAN | 3                                                      \
  }

#define FALSE_VAL                                                              \
  (value_t)                                                                    \
  {                                                                            \
    .as_int = BOOLEAN | 2                                                      \
  }

#define NIL_VAL                                                                \
  (value_t)                                                                    \
  {                                                                            \
    .as_int = NIL                                                              \
  }

#define SEXPR 0xfffc000000000000

void print_value(value_t value);
value_type_t type_of(value_t value);
bool values_equal(value_t v1, value_t v2);

#endif