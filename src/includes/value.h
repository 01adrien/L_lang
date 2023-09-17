#ifndef _VALUE_H_
#define _VALUE_H_

#include "common.h"

typedef union {
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

/*
typedef uint64_t value_t;


#define NANISH  0xffff000000000000
#define BOOLEAN 0x7ffe000000000002
#define INTEGER 0x7ffc000000000000
#define OBJECT  0xfffc000000000000
#define STRING  0xfffe000000000000

#define TRUE_VAL  (BOOLEAN | 3)
#define FALSE_VAL (BOOLEAN | 2)
#define NIL_VAL   0x7ffe000000000000

#define EQ(v1, v2) ((v1) == (v2))

#define IS_OBJECT(v)  ((v & NANISH) == OBJECT)
#define IS_STRING(v)  ((v & NANISH) == STRING)
#define IS_NIL_VAL(v)     (v == NIL_VAL)
#define IS_BOOLEAN(v) ((v & BOOLEAN) == BOOLEAN)
#define IS_NUMBER(v)  ((v & NANISH) == INTEGER)

#define AS_BOOLEAN(v) ((int8_t) (v & 0x1))
#define AS_NUMBER(v)  ((int32_t) (v & 0xFFFFFFFFFFFF))
#define AS_OBJECT(v)  ((void*) (v & 0xFFFFFFFFFFFF))
#define AS_STRING(v)  ((char*) (v & 0xFFFFFFFFFFFF))

#define NUMBER_VAL(i) ((uint64_t) (i) | INTEGER)
#define OBJECT_VAL(p) ((uint64_t) (p) | OBJECT)
#define STRING_VAL(p) ((uint64_t) (p) | STRING)

*/
/*
typedef uint64_t value_t;

#define OUR_NAN(v)   ((v & 0x7ffc000000000000) == 0x7ffc000000000000)
#define NAN     0xffff000000000000
#define NAN_VALUE(v) (v & (~NAN))

#define EQ(v1, v2) ((v1) == (v2))

#define NIL_VAL       0x7ffd000000000000
#define IS_NIL_VAL(v) ((v & NIL_VAL) == NIL_VAL)

// Strings
#define STRING        0x7fff000000000000
#define IS_STRING(v)  ((v & NAN) == STRING)
#define AS_STRING(v)  ((char*) (NAN_VALUE(v)))
#define STRING_VAL(p) (*(uint64_t*) (&p) | STRING)

// Int 32
#define NUMBER        0x7ffc000000000000
#define IS_NUMBER(v)  ((v & NAN) == NUMBER)
#define NUMBER_VAL(p) ((uint64_t) (p) | NUMBER)
#define AS_NUMBER(v)  ((int32_t) (NAN_VALUE(v)))

// Boolean
#define BOOLEAN       0x7ffe000000000002
#define TRUE_VAL          (BOOLEAN | 3)
#define FALSE_VAL         (BOOLEAN | 2)
#define IS_BOOLEAN(v) ((v & BOOLEAN) == BOOLEAN)
#define AS_BOOLEAN(v) ((int8_t) (v & 0x1))

#define PRIM 0xfffc000000000000
*/

#endif   // !1