#ifndef _VM_H_
#define _VM_H_

#include "chunk.h"
#include "common.h"
#include "object.h"
#include "table.h"

#define MAX_STACK     0xFF
#define MEMORY_SIZE   0x10000
#define MAX_TRACE_LEN 16

typedef enum interpret_result {
  SUCCESS,
  ERROR_DIVISION_BY_ZERO,
  ERROR_RUNTIME_EXCEPTION,
  ERROR_UNKNOWN_OPCODE,
  ERROR_END_OF_STREAM,
  ERROR_STACK_OVERFLOW,
} interpret_result_t;

typedef struct vm {
  uint8_t* ip;
  value_t* stack_top;
  value_t stack[MAX_STACK];
  chunk_t* chunk;
  table_t strings;
  table_t globals;
  object_t* objects;
} vm_t;

extern vm_t vm;

interpret_result_t run_vm(chunk_t* chunk);
void free_vm();
void init_vm();
char* interpreter_status(interpret_result_t res);

#endif