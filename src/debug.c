#include "includes/debug.h"
#include "includes/value.h"
#include <stdio.h>

int basic_instruction(const char* name, int offset);
int constant_8_instruction(const char* name, int offset, chunk_t* chunk);
int constant_16_instruction(const char* name, int offset, chunk_t* chunk);
int byte_instruction(const char* name, int offset, chunk_t* chunk);

void debug_chunk(chunk_t* chunk, const char* name)
{
  printf("== %s ==\n", name);
  int offset = 0;
  while (offset < chunk->count) {
    offset = debug_instruction(chunk, offset);
  }
  printf("constants count %ld\n", chunk->constants.count);
}

int debug_instruction(chunk_t* chunk, int offset)
{
  // implement constant 16
  printf("%4d | ", offset);
  OPcode_t instruction = chunk->OPcodes[offset];
  switch (instruction) {
  case OP_RETURN:
    return basic_instruction("OP_RETURN", offset);
  case OP_CONSTANT_8:
    return constant_8_instruction("OP_CONSTANT_8", offset, chunk);
  case OP_CONSTANT_16:
    return constant_8_instruction("OP_CONSTANT_16", offset, chunk);
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
  case OP_PRINT:
    return basic_instruction("OP_PRINT", offset);
  case OP_POP:
    return basic_instruction("OP_POP", offset);
  case OP_DEFINE_GLOBAL:
    return constant_8_instruction("OP_DEFINE_GLOBAL", offset, chunk);
  case OP_GET_GLOBAL:
    return constant_8_instruction("OP_GET_GLOBAL", offset, chunk);
  case OP_SET_GLOBAL:
    return constant_8_instruction("OP_SET_GLOBAL", offset, chunk);
  case OP_GET_LOCAL:
    return byte_instruction("OP_GET_LOCAL", offset, chunk);
    break;
  case OP_SET_LOCAL:
    return byte_instruction("OP_SET_LOCAL", offset, chunk);
  default:
    printf("unknow OP code. [%d]\n", instruction);
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

int byte_instruction(const char* name, int offset, chunk_t* chunk)
{
  printf("%s : ", name);
  printf("%d \n", chunk->OPcodes[++offset]);
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
