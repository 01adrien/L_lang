#include "includes/chunk.h"
#include "includes/memory.h"
#include <stdio.h>

chunk_t* create_chunk()
{
  chunk_t* c = calloc(1, sizeof(chunk_t));
  c->capacity = 0;
  c->count = 0;
  c->OPcodes = NULL;
  c->constants = create_value_array();
  return c;
}

void free_chunk(chunk_t* chunk)
{
  // free constant
  free(chunk->OPcodes);
}

void write_chunk(chunk_t* chunk, uint8_t byte)
{
  if (chunk->capacity < chunk->count + 1) {
    int old_capacity = chunk->capacity;
    chunk->capacity = GROW_ARRAY_CAPACITY(old_capacity);
    chunk->OPcodes = GROW_ARRAY_MEMORY(
        uint8_t, chunk->OPcodes, old_capacity, chunk->capacity
    );
  }
  chunk->OPcodes[chunk->count] = byte;
  chunk->count++;
}

value_array_t create_value_array()
{
  value_array_t v;
  v.capacity = 0;
  v.count = 0;
  v.values = NULL;
  return v;
}

uint16_t write_constant(chunk_t* chunk, value_t value)
{
  write_value_array(&chunk->constants, value);
  return chunk->constants.count - 1;
}

void write_value_array(value_array_t* array, value_t value)
{
  if (array->capacity < array->count + 1) {
    int old_capacity = array->capacity;
    array->capacity = GROW_ARRAY_CAPACITY(old_capacity);
    array->values = GROW_ARRAY_MEMORY(
        value_t, array->values, old_capacity, array->capacity
    );
  }
  array->values[array->count] = value;
  array->count++;
}

void emit_byte(chunk_t* chunk, uint8_t byte)
{
  write_chunk(chunk, byte);
}

void emit_bytes(chunk_t* chunk, uint8_t byte1, uint8_t byte2)
{
  emit_byte(chunk, byte1);
  emit_byte(chunk, byte2);
}

void emit_constant(chunk_t* chunk, value_t value)
{
  uint16_t index = write_constant(chunk, value);
  if (index > INT8_MAX) {
    uint8_t b1 = index >> 8;
    uint8_t b2 = index << 8;
    emit_byte(chunk, OP_CONSTANT_16);
    emit_bytes(chunk, b1, b2);
  }
  else {
    emit_bytes(chunk, OP_CONSTANT_8, index);
  }
}