#include "includes/compiler.h"
#include "includes/chunk.h"
#include "includes/debug.h"
#include "includes/memory.h"
#include "includes/value.h"
#include <stdarg.h>
#include <stdio.h>

value_t string(token_t token);
uint16_t parse_variable(token_t token, compiler_t* compiler);
int resolve_local(token_t name, compiler_t* compiler);
uint16_t get_constant_index(chunk_t* chunk, token_t token);
void declare_local(token_t name, compiler_t* compiler);
bool id_equal(token_t id1, token_t id2);
void compile_to(token_type_t token, compiler_t* compiler);
void begin_block(compiler_t* compiler);
void end_block(compiler_t* compiler);
void var_declaration(compiler_t* compiler);
void get_set_id(token_t token, compiler_t* compiler);
void compile(compiler_t* compiler);
void error_compiler(const char* msg, ...);

void init_compiler(compiler_t* compiler, chunk_t* chunk, token_queue_t* queue)
{
  compiler->local_count = 0;
  compiler->scope_depth = 0;
  compiler->chunk = chunk;
  compiler->queue = queue;
  compiler->is_error = false;
}

void compile(compiler_t* compiler)
{
  token_queue_t* queue = compiler->queue;
  chunk_t* chunk = compiler->chunk;
  token_t token = get_token(dequeue_token(queue));

  switch (token.type) {
  case TOKEN_MINUS:
    emit_byte(chunk, OP_SUBSTRACT);
    break;
  case TOKEN_PLUS:
    emit_byte(chunk, OP_ADD);
    break;
  case TOKEN_STAR:
    emit_byte(chunk, OP_MULTIPLY);
    break;
  case TOKEN_MINUS_UNARY:
    emit_byte(chunk, OP_NEGATE);
    break;
  case TOKEN_SLASH:
    emit_byte(chunk, OP_DIVIDE);
    break;
  case TOKEN_MODULO:
    emit_byte(chunk, OP_MODULO);
    break;
  case TOKEN_GREATER:
    emit_byte(chunk, OP_GREATER);
    break;
  case TOKEN_GREATER_EQUAL:
    emit_bytes(chunk, OP_LESS, OP_NOT);
    break;
  case TOKEN_LESS:
    emit_byte(chunk, OP_LESS);
    break;
  case TOKEN_LESS_EQUAL:
    emit_bytes(chunk, OP_GREATER, OP_NOT);
    break;
  case TOKEN_BANG_EQUAL:
    emit_bytes(chunk, OP_EQUAL, OP_NOT);
    break;
  case TOKEN_EQUAL_EQUAL:
    emit_byte(chunk, OP_EQUAL);
    break;
  case TOKEN_BANG:
    emit_byte(chunk, OP_NOT);
    break;
  case TOKEN_NUMBER:
    emit_constant(chunk, NUMBER_VAL(strtod(token.start, NULL)));
    break;
  case TOKEN_STRING:
    emit_constant(chunk, string(token));
    break;
  case TOKEN_TRUE:
    emit_byte(chunk, OP_TRUE);
    break;
  case TOKEN_FALSE:
    emit_byte(chunk, OP_FALSE);
    break;
  case TOKEN_NIL:
    emit_byte(chunk, OP_NIL);
    break;
  case TOKEN_PRINT:
    compile_to(TOKEN_SEMICOLON, compiler);
    emit_byte(chunk, OP_PRINT);
    break;
  case TOKEN_VAR:
    var_declaration(compiler);
    break;
  case TOKEN_IDENTIFIER:
    get_set_id(token, compiler);
    break;
  case TOKEN_LEFT_BRACE:
    begin_block(compiler);
    break;
  case TOKEN_RIGHT_BRACE:
    end_block(compiler);
    break;
  case TOKEN_RIGHT_PAREN:
  case TOKEN_LEFT_PAREN:
  case TOKEN_SEMICOLON:
    break;
  case TOKEN_NEWLINE:
    compile_to(TOKEN_SEMICOLON, compiler);
    break;
  default:
    return;
  }
}

compile_error_t compilation(compiler_t* compiler)
{
  while (compiler->queue->head) {
    compile(compiler);
    if (compiler->is_error) {
      return "compilation error";
    }
  }
  emit_byte(compiler->chunk, OP_RETURN);
}

void compile_to(token_type_t token, compiler_t* compiler)
{
  token_queue_t* queue = compiler->queue;
  if (!queue->head) {
    return;
  }
  while (queue->head && queue->head->token.type != token) {
    compile(compiler);
  }
}

void begin_block(compiler_t* compiler)
{
  compiler->scope_depth++;
  compile_to(TOKEN_RIGHT_BRACE, compiler);
}

void end_block(compiler_t* compiler)
{
  chunk_t* chunk = compiler->chunk;
  compiler->scope_depth--;
  while (compiler->local_count > 0
         && compiler->locals[compiler->local_count - 1].depth
                > compiler->scope_depth) {
    emit_byte(chunk, OP_POP);
    compiler->local_count--;
  }
}

void var_declaration(compiler_t* compiler)
{
  token_queue_t* queue = compiler->queue;
  chunk_t* chunk = compiler->chunk;
  token_t name = get_token(dequeue_token(queue));
  uint16_t index = parse_variable(name, compiler);

  if (queue->head->token.type == TOKEN_EQUAL) {
    FREE(dequeue_token(queue), node_t);
    compile_to(TOKEN_SEMICOLON, compiler);
  }
  else {
    emit_byte(chunk, OP_NIL);
  }
  if (compiler->scope_depth == 0) {
    emit_bytes(chunk, OP_DEFINE_GLOBAL, index);
  }
}

void get_set_id(token_t token, compiler_t* compiler)
{
  token_queue_t* queue = compiler->queue;
  chunk_t* chunk = compiler->chunk;
  uint16_t get_OP, set_OP;
  int index = resolve_local(token, compiler);

  if (index == -1) {
    index = get_constant_index(chunk, token);
    get_OP = OP_GET_GLOBAL;
    set_OP = OP_SET_GLOBAL;
  }
  else {
    get_OP = OP_GET_LOCAL;
    set_OP = OP_SET_LOCAL;
  }
  if (queue->head->token.type == TOKEN_EQUAL) {
    FREE(dequeue_token(queue), node_t);
    compile_to(TOKEN_SEMICOLON, compiler);
    emit_bytes(chunk, set_OP, (uint16_t) index);
  }
  else {
    emit_bytes(chunk, get_OP, (uint16_t) index);
  }
}

uint16_t parse_variable(token_t token, compiler_t* compiler)
{
  chunk_t* chunk = compiler->chunk;
  declare_local(token, compiler);
  if (compiler->scope_depth > 0) {
    return 0;
  }
  return get_constant_index(chunk, token);
}

uint16_t get_constant_index(chunk_t* chunk, token_t token)
{
  char* s = (char*) calloc(token.length, sizeof(char));
  memcpy(s, token.start, token.length);
  value_t str = get_string(s);
  return write_constant(chunk, str);
}

void declare_local(token_t name, compiler_t* compiler)
{
  if (compiler->scope_depth == 0) {
    return;
  }
  if (compiler->local_count == UINT8_COUNT) {
    fprintf(stderr, "too many local variables in function.");
    compiler->is_error = true;
  }
  for (int i = compiler->local_count - 1; i >= 0; i--) {
    local_t local = compiler->locals[i];
    if (id_equal(local.name, name) && local.depth == compiler->scope_depth) {
      error_compiler(
          "already a variable with name '%.*s' in this scope at line %d",
          name.length, name.start, name.line
      );
      compiler->is_error = true;
    }
  }
  local_t* local = &compiler->locals[compiler->local_count++];
  local->depth = compiler->scope_depth;
  local->name = name;
}

int resolve_local(token_t name, compiler_t* compiler)
{
  for (int i = compiler->local_count - 1; i >= 0; i--) {
    local_t local = compiler->locals[i];
    if (id_equal(local.name, name)) {
      return i;
    }
  }
  return -1;
}

bool id_equal(token_t id1, token_t id2)
{
  return id1.length != id2.length
             ? false
             : memcmp(id1.start, id2.start, id1.length) == 0;
}

value_t string(token_t token)
{
  char* str = (char*) calloc(token.length - 2, sizeof(char));
  memcpy(str, token.start + 1, token.length - 2);
  return get_string(str);
}

void error_compiler(const char* msg, ...)
{
  va_list args;
  va_start(args, msg);
  va_end(args);
  fprintf(stderr, "Error: ");
  vfprintf(stderr, msg, args);
  printf("\n");
}
