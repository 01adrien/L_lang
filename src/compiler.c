#include "includes/compiler.h"
#include "includes/chunk.h"
#include "includes/memory.h"
#include "includes/value.h"
#include <stdio.h>

value_t string(token_t token);
uint16_t get_constant_var(chunk_t* chunk, token_t token);
void compile_one_btc(token_queue_t* queue, chunk_t* chunk);
void compile_expression_btc(token_queue_t* queue, chunk_t* chunk);

void compile_btc(token_queue_t* queue, chunk_t* chunk)
{
  while (queue->head) {
    compile_one_btc(queue, chunk);
  }
  emit_byte(chunk, OP_RETURN);
}

void compile_expression_btc(token_queue_t* queue, chunk_t* chunk)
{
  while (queue->head->token.type != TOKEN_SEMICOLON) {
    compile_one_btc(queue, chunk);
  }
}

void compile_one_btc(token_queue_t* queue, chunk_t* chunk)
{
  if (queue->head) {
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
    case TOKEN_PRINT: {
      compile_expression_btc(queue, chunk);
      emit_byte(chunk, OP_PRINT);
      break;
    }
    case TOKEN_VAR: {
      token_t name = get_token(dequeue_token(queue));
      uint16_t index = get_constant_var(chunk, name);
      token_t next = get_token(dequeue_token(queue));
      if (next.type == TOKEN_EQUAL) {
        compile_expression_btc(queue, chunk);
        emit_bytes(chunk, OP_DEFINE_GLOBAL, index);
      }
      else {
        emit_byte(chunk, OP_NIL);
      }
      break;
    }
    case TOKEN_IDENTIFIER: {
      uint16_t index = get_constant_var(chunk, token);
      if (queue->head->token.type == TOKEN_EQUAL) {
        FREE(dequeue_token(queue), node_t);
        compile_expression_btc(queue, chunk);
        emit_bytes(chunk, OP_SET_GLOBAL, index);
      }
      else {
        emit_bytes(chunk, OP_GET_GLOBAL, index);
      }
      break;
    }
    default:
      return;
    }
  }
}

uint16_t get_constant_var(chunk_t* chunk, token_t token)
{
  char* s = calloc(token.length, sizeof(char));
  memcpy(s, token.start, token.length);
  value_t str = get_string(s);
  return write_constant(chunk, str);
}

value_t string(token_t token)
{
  char* str = calloc(token.length - 2, sizeof(char));
  memcpy(str, token.start + 1, token.length - 2);
  return get_string(str);
}