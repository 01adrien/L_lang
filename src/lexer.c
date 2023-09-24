#include "includes/lexer.h"
#include <stdio.h>

token_t token_number(scanner_t* scanner);
token_t token_string(scanner_t* scanner);
token_t token_error(const char* msg, scanner_t* scanner);
token_t token_identifier(scanner_t* scanner);
token_type_t check_id(scanner_t* scanner);
token_type_t is_keyword(
    const char* keyword, int len, token_type_t token_type, scanner_t* scanner
);

token_type_t is_keyword(
    const char* keyword, int len, token_type_t token_type, scanner_t* scanner
)
{
  return memcmp(scanner->start, keyword, len) == 0 ? token_type
                                                   : TOKEN_IDENTIFIER;
}

token_type_t check_id(scanner_t* scanner)
{
  switch (scanner->start[0]) {
  case 't':
    return is_keyword("true", 4, TOKEN_TRUE, scanner);
  case 'f':
    return is_keyword("false", 5, TOKEN_FALSE, scanner);
  case 'n':
    return is_keyword("nil", 3, TOKEN_NIL, scanner);
  case 'v':
    return is_keyword("var", 3, TOKEN_VAR, scanner);
  case 'p':
    return is_keyword("print", 4, TOKEN_PRINT, scanner);
  }
  return TOKEN_IDENTIFIER;
}

token_t scan_token(scanner_t* scanner)
{
  skip_blanks(scanner);
  scanner->start = scanner->current;
  if (end_file(scanner)) {
    return make_token(TOKEN_EOF, scanner, PREC_NONE);
  }
  char c = advance_scanner(scanner);
  if (isdigit(c)) {
    return token_number(scanner);
  }
  if (isalpha(c)) {
    return token_identifier(scanner);
  }
  switch (c) {
  case '+':
    return make_token(TOKEN_PLUS, scanner, PREC_TERM);
  case '-':
    return make_token(TOKEN_MINUS, scanner, PREC_TERM);
  case '*':
    return make_token(TOKEN_STAR, scanner, PREC_FACTOR);
  case '/':
    return make_token(TOKEN_SLASH, scanner, PREC_FACTOR);
  case '%':
    return make_token(TOKEN_MODULO, scanner, PREC_FACTOR);
  case '(':
    return make_token(TOKEN_LEFT_PAREN, scanner, PREC_NONE);
  case ')':
    return make_token(TOKEN_RIGHT_PAREN, scanner, PREC_NONE);
  case '{':
    return make_token(TOKEN_LEFT_BRACE, scanner, PREC_NONE);
  case '}':
    return make_token(TOKEN_RIGHT_BRACE, scanner, PREC_NONE);
  case '"':
    return token_string(scanner);
  case '>':
    return make_token(
        match(scanner, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER, scanner,
        PREC_COMPARISON
    );
  case '<':
    return make_token(
        match(scanner, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS, scanner,
        PREC_COMPARISON
    );
  case '=':
    return make_token(
        match(scanner, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL, scanner,
        PREC_EQUALITY
    );
  case '!':
    return make_token(
        match(scanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG, scanner,
        PREC_EQUALITY
    );
  case ';':
    scanner->line++;
    return make_token(TOKEN_SEMICOLON, scanner, PREC_NONE);
  case '\n':
    return make_token(TOKEN_NEWLINE, scanner, PREC_NONE);
  case '$':
  case '.':
    break;
  default:
    printf("[ %c ]\n", c);
    return token_error("unknow token.", scanner);
  }
}

token_t token_number(scanner_t* scanner)
{

  while (isdigit(peek_scanner(scanner))) {
    advance_scanner(scanner);
    if (peek_scanner(scanner) == '.') {
      advance_scanner(scanner);
      while (isdigit(peek_scanner(scanner))) {
        advance_scanner(scanner);
      }
      // handle float here with another token
    }
  }
  return make_token(TOKEN_NUMBER, scanner, PREC_NONE);
}

token_t token_string(scanner_t* scanner)
{
  while (peek_scanner(scanner) != '"' && !end_file(scanner)) {
    advance_scanner(scanner);
  }
  if (end_file(scanner)) {
    return token_error("unterminated string", scanner);
  }
  advance_scanner(scanner);
  return make_token(TOKEN_STRING, scanner, PREC_NONE);
}

token_t token_identifier(scanner_t* scanner)
{
  while (isalpha(peek_scanner(scanner))) {
    advance_scanner(scanner);
  }
  return make_token(check_id(scanner), scanner, PREC_NONE);
}

token_t
make_token(token_type_t type, scanner_t* scanner, precedence_t precedence)
{
  token_t token;
  token.start = scanner->start;
  token.length = (scanner->current - scanner->start);
  token.type = type;
  token.line = scanner->line;
  token.precedence = precedence;
  return token;
}

token_t token_error(const char* msg, scanner_t* scanner)
{
  token_t token;
  token.start = msg;
  token.length = strlen(msg);
  token.type = TOKEN_ERROR;
  token.line = scanner->line;
  return token;
}

void advance_lexer(lexer_t* lexer, scanner_t* scanner)
{
  lexer->previous = lexer->current;
  lexer->current = lexer->next;
  lexer->next = scan_token(scanner);
}

char* type(token_t token)
{
  switch (token.type) {
  case TOKEN_EOF:
    return "EOF";
  case TOKEN_FALSE:
    return "FALSE_VAL";
  case TOKEN_TRUE:
    return "TRUE_VAL";
  case TOKEN_ERROR:
    return "ERROR";
  case TOKEN_STRING:
    return "STRING";
  case TOKEN_NUMBER:
    return "NUMBER";
  case TOKEN_LEFT_PAREN:
    return "LEFT PAREN";
  case TOKEN_RIGHT_PAREN:
    return "RIGHT PAREN";
  case TOKEN_RIGHT_BRACE:
    return "RIGHT BRACE";
  case TOKEN_LEFT_BRACE:
    return "LEFT BRACE";
  case TOKEN_IDENTIFIER:
    return "IDENTIFIER";
  case TOKEN_NEWLINE:
    return "NEWLINE";
  case TOKEN_SEMICOLON:
    return "SEMICOLON";
  case TOKEN_MODULO:
    return "MODULO";
  case TOKEN_SLASH:
    return "SLASH";
  case TOKEN_STAR:
    return "STAR";
  case TOKEN_MINUS:
    return "MINUS";
  case TOKEN_PLUS:
    return "PLUS";
  case TOKEN_NIL:
    return "NIL_VAL";
  case TOKEN_LESS:
    return "LESS";
  case TOKEN_GREATER:
    return "GREATER";
  case TOKEN_EQUAL:
    return "EQUAL";
  case TOKEN_LESS_EQUAL:
    return "LESS EQUAL";
  case TOKEN_GREATER_EQUAL:
    return "GREATER EQUAL";
  case TOKEN_EQUAL_EQUAL:
    return "EQUAL EQUAL";
  case TOKEN_BANG_EQUAL:
    return "BANG EQUAL";
  case TOKEN_BANG:
    return "BANG";
  case TOKEN_MINUS_UNARY:
    return "MINUS UNARY";
  case TOKEN_EMPTY:
    return "EMPTY";
  case TOKEN_VAR:
    return "VAR";
  case TOKEN_PRINT:
    return "PRINT";
  default:
    return "UNKNOW";
  }
}