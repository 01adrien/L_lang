#ifndef _PARSER_H_
#define _PARSER_H_

#include "common.h"
#include "lexer.h"
#include "scanner.h"
#include "vm.h"

typedef const char* parsing_error_t;
typedef struct node node_t;

struct node {
  token_t token;
  node_t* next;
};

typedef struct token_stack {
  node_t* top;
} token_stack_t;

typedef struct token_queue {
  node_t* head;
} token_queue_t;

typedef struct parser {
  lexer_t* lexer;
  scanner_t* scanner;
  chunk_t* chunk;
  bool is_error;
  parsing_error_t error;
  token_queue_t* queue;
  token_stack_t* stack;
} parser_t;

void init_parser(
    lexer_t* lexer, scanner_t* scanner, parser_t* parser, token_stack_t* stack,
    token_queue_t* queue
);
parsing_error_t parse(parser_t* parser);
token_t get_token(node_t* node);
node_t* dequeue_token(token_queue_t* queue);

#endif   // !_PARSER_H_