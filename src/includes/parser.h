#ifndef _PARSER_H_
#define _PARSER_H_

#include "chunk.h"
#include "common.h"
#include "lexer.h"
#include "scanner.h"

typedef const char* parsing_error_t;

typedef struct parser {
  lexer_t* lexer;
  scanner_t* scanner;
  chunk_t* chunk;
  bool is_error;
  parsing_error_t error;
} parser_t;

parsing_error_t parse(parser_t* parser, precedence_t precedence);
void init_parser(
    lexer_t* lexer, scanner_t* scanner, parser_t* parser, chunk_t* chunk
);

#endif   // !_PARSER_H_