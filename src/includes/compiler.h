#ifndef _COMPILER_H_
#define _COMPILER_H_

#include "common.h"
#include "parser.h"

typedef char* compile_error_t;

typedef struct {
  token_t name;
  int depth;
} local_t;

typedef struct {
  local_t locals[UINT8_COUNT];
  int local_count;
  int scope_depth;
  token_queue_t* queue;
  chunk_t* chunk;
  bool is_error;
} compiler_t;

compile_error_t compilation(compiler_t* compiler);
void init_compiler(compiler_t* compiler, chunk_t* chunk, token_queue_t* queue);

#endif