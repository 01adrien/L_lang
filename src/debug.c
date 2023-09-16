#include "includes/debug.h"
#include "includes/value.h"
#include <stdio.h>

int basic_instruction(const char* name, int offset);
int constant_8_instruction(const char* name, int offset, chunk_t* chunk);
int constant_16_instruction(const char* name, int offset, chunk_t* chunk);

void debug_chunk(chunk_t* chunk, const char* name)
{
  printf("== %s ==\n", name);
  int offset = 0;
  while (offset < chunk->count) {
    offset = debug_instruction(chunk, offset);
  }
}

int debug_instruction(chunk_t* chunk, int offset)
{
  printf("%2d | ", offset);
  OPcode_t instruction = chunk->OPcodes[offset];
  switch (instruction) {
  case OP_RETURN:
    return basic_instruction("OP_RETURN", offset);
  case OP_CONSTANT_8:
    return constant_8_instruction("OP_CONSTANT_8", offset, chunk);
  case OP_ADD:
    return basic_instruction("OP_ADD", offset);
  case OP_DIVIDE:
    return basic_instruction("OP_DIVIDE", offset);
  case OP_MULTIPLY:
    return basic_instruction("OP_MULTIPLY", offset);
  case OP_SUBSTRACT:
    return basic_instruction("OP_SUBSTRACT", offset);
  case OP_NEGATE:
    return basic_instruction("OP_NEGATE", offset);
  case OP_TRUE:
    return basic_instruction("OP_TRUE", offset);
  case OP_FALSE:
    return basic_instruction("OP_FALSE", offset);
  case OP_NIL:
    return basic_instruction("OP_NIL", offset);
  case OP_LESS:
    return basic_instruction("OP_LESS", offset);
  case OP_GREATER:
    return basic_instruction("OP_GREATER", offset);
  case OP_EQUAL:
    return basic_instruction("OP_EQUAL", offset);
  case OP_NOT:
    return basic_instruction("OP_NOT", offset);
  case OP_MODULO:
    return basic_instruction("OP_MODULO", offset);
  default:
    printf("unknow OP code.\n");
    break;
  }
}

int basic_instruction(const char* name, int offset)
{
  printf("%s\n", name);
  return offset + 1;
}

int constant_8_instruction(const char* name, int offset, chunk_t* chunk)
{
  printf("%s : ", name);
  print_value(chunk->constants.values[chunk->OPcodes[++offset]]);
  printf("\n");
  return offset + 1;
}

int constant_16_instruction(const char* name, int offset, chunk_t* chunk)
{
  uint8_t b1 = chunk->OPcodes[++offset];
  uint8_t b2 = chunk->OPcodes[++offset];
  uint16_t index = ((b2) << 8) | ((b2) >> 8);
  printf("%s : ", name);
  print_value(chunk->constants.values[chunk->OPcodes[index]]);
  printf("\n");
  return offset + 1;
}
