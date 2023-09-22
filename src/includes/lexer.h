#ifndef _LEXER_H_
#define _LEXER_H_

#include "common.h"
#include "scanner.h"

typedef enum token_type {
  TOKEN_EMPTY,
  TOKEN_EOF,
  TOKEN_NEWLINE,
  TOKEN_SEMICOLON,
  TOKEN_SLASH,
  TOKEN_STAR,
  TOKEN_MINUS,
  TOKEN_MINUS_UNARY,
  TOKEN_PLUS,
  TOKEN_NUMBER,
  TOKEN_MODULO,
  TOKEN_LESS,
  TOKEN_GREATER,
  TOKEN_EQUAL,
  TOKEN_LESS_EQUAL,
  TOKEN_GREATER_EQUAL,
  TOKEN_EQUAL_EQUAL,
  TOKEN_BANG,
  TOKEN_BANG_EQUAL,
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE,
  TOKEN_RIGHT_BRACE,
  TOKEN_NIL,
  TOKEN_ERROR,
  TOKEN_FALSE,
  TOKEN_TRUE,
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_VAR,
  TOKEN_PRINT,
} token_type_t;

typedef enum precedence {
  PREC_NONE,
  PREC_ASSIGNMENT,   // =
  PREC_OR,           // or
  PREC_AND,          // and
  PREC_EQUALITY,     // == !=
  PREC_COMPARISON,   // < > <= >=
  PREC_TERM,         // + -
  PREC_FACTOR,       // * / %
  PREC_UNARY,        // ! -
  PREC_CALL,         // . ()
  PREC_PRIMARY
} precedence_t;

typedef struct token {
  token_type_t type;
  precedence_t precedence;
  const char* start;
  int length;
  int line;
} token_t;

typedef struct lexer {
  token_t previous;
  token_t current;
  token_t next;
} lexer_t;

#define PRINT_TOKEN(token)                                                     \
  printf("[TOKEN %s] %.*s\n", type(token), (token).length, (token).start)

token_t scan_token(scanner_t* scanner);
void advance_lexer(lexer_t* lexer, scanner_t* scanner);
char* type(token_t token);
token_t
make_token(token_type_t type, scanner_t* scanner, precedence_t precedence);

#endif   //