#ifndef _CHUNK_H_
#define _CHUNK_H_

#include "common.h"
#include "value.h"

typedef enum {
  OP_CONSTANT_8,
  OP_CONSTANT_16,
  OP_RETURN,
  OP_ADD,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_SUBSTRACT,
  OP_NEGATE,
  OP_MODULO,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  OP_NOT,
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_PRINT,
  OP_POP,
  OP_DEFINE_GLOBAL,
  OP_GET_GLOBAL,
  OP_SET_GLOBAL,
  OP_SET_LOCAL,
  OP_GET_LOCAL,
  OP_COUNT,
} OPcode_t;

typedef struct value_array {
  size_t count;
  size_t capacity;
  value_t* values;
} value_array_t;

typedef struct chunk {
  size_t count;
  size_t capacity;
  uint8_t* OPcodes;
  value_array_t constants;
} chunk_t;

chunk_t* create_chunk();
value_array_t create_value_array();
void write_chunk(chunk_t* chunk, uint8_t byte);
void free_chunk(chunk_t* chunk);
uint16_t write_constant(chunk_t* chunk, value_t value);
void write_value_array(value_array_t* array, value_t value);
void free_value_array(value_array_t* array);
void emit_byte(chunk_t* chunk, uint8_t byte);
void emit_bytes(chunk_t* chunk, uint8_t byte1, uint8_t byte2);
void emit_constant(chunk_t* chunk, value_t value);
#endif   // !