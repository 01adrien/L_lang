#include "includes/parser.h"
#include <stdio.h>

void expression(parser_t* parser);
void binary(parser_t* parser);
void literal(parser_t* parser);
void grouping(parser_t* parser);
void unary(parser_t* parser);

// Helpers
void consume(parser_t* parser, token_type_t type, parsing_error_t error);

// Shuting yard

void init_parser(
    lexer_t* lexer, scanner_t* scanner, parser_t* parser, chunk_t* chunk,
    token_stack_t* stack, token_queue_t* queue
)
{
  parser->lexer = lexer;
  parser->scanner = scanner;
  parser->chunk = chunk;
  parser->is_error = false;
  parser->error = NULL;
  parser->queue = queue;
  parser->queue->head = NULL;
  parser->stack = stack;
  parser->stack->top = NULL;
}

parsing_error_t parse(parser_t* parser, precedence_t precedence)
{

  lexer_t* lexer = parser->lexer;
  scanner_t* scanner = parser->scanner;
  while (lexer->current.type != TOKEN_EOF) {
    advance_lexer(lexer, scanner);
    expression(parser);
  }
}

void expression(parser_t* parser)
{
  lexer_t* lexer = parser->lexer;
  token_t token = lexer->previous;
  switch (token.type) {
  case TOKEN_STRING:
  case TOKEN_TRUE:
  case TOKEN_FALSE:
  case TOKEN_NIL:
  case TOKEN_NUMBER:
    enqueue_token(parser, token);
    break;
  case TOKEN_BANG_EQUAL:
  case TOKEN_GREATER:
  case TOKEN_GREATER_EQUAL:
  case TOKEN_LESS:
  case TOKEN_LESS_EQUAL:
  case TOKEN_EQUAL_EQUAL:
  case TOKEN_MODULO:
  case TOKEN_MINUS:
  case TOKEN_PLUS:
  case TOKEN_STAR:
  case TOKEN_SLASH:
    push_token(parser, token);
    break;
  case TOKEN_BANG:
    unary(parser);
    break;
  case TOKEN_LEFT_PAREN:
    grouping(parser);
    break;
  case TOKEN_RIGHT_PAREN:
    break;
  case TOKEN_EOF:
    return;
  default:
    printf("Unknow token : ");
    PRINT_TOKEN(lexer->previous);
    break;
  }
}

void binary(parser_t* parser)
{
  token_t token = parser->lexer->previous;
  chunk_t* chunk = parser->chunk;
  parse(parser, ((precedence_t) (token.precedence + 1)));
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
  default:
    return;
  }
}

void literal(parser_t* parser)
{
  token_t token = parser->lexer->previous;
  chunk_t* chunk = parser->chunk;
  switch (token.type) {
  case TOKEN_NUMBER:
    emit_constant(chunk, NUMBER_VAL(strtod(token.start, NULL)));
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
  default:
    return;
  }
}

void grouping(parser_t* parser)
{
  parse(parser, PREC_ASSIGNMENT);
  consume(parser, TOKEN_RIGHT_PAREN, "Expect ')' after expression.\n");
}

void unary(parser_t* parser)
{
  token_t token = parser->lexer->previous;
  chunk_t* chunk = parser->chunk;
  parse(parser, PREC_UNARY);
  switch (token.type) {
  case TOKEN_BANG:
    emit_byte(chunk, OP_NOT);
    break;

  default:
    break;
  }
}

void print_queue(token_queue_t* queue)
{
  node_t* temp = queue->head;
  while (temp) {
    PRINT_TOKEN(temp->token);
    temp = temp->next;
  }
}

void print_stack(token_stack_t* stack)
{
  node_t* temp = stack->top;
  while (temp) {
    PRINT_TOKEN(temp->token);
    temp = temp->next;
  }
}

void free_queue(token_queue_t* queue)
{
}

void free_stack(token_stack_t* stack)
{
}

void enqueue_token(parser_t* parser, token_t token)
{
  token_queue_t* queue = parser->queue;
  token_stack_t* stack = parser->stack;
  node_t* new = calloc(1, sizeof(node_t));
  new->token = token;
  new->next = NULL;

  node_t* temp = queue->head;
  if (!temp) {
    queue->head = new;
    return;
  }
  while (temp->next) {
    temp = temp->next;
  }
  temp->next = new;
}

void push_token(parser_t* parser, token_t token)
{
  token_queue_t* queue = parser->queue;
  token_stack_t* stack = parser->stack;
  node_t* new = calloc(1, sizeof(node_t));
  new->token = token;
  new->next = NULL;
  while (stack->top && stack->top->token.precedence > token.precedence) {
    enqueue_token(parser, pop_token(stack)->token);
  }
  node_t* temp = stack->top;
  if (!temp) {
    stack->top = new;
    return;
  }
  new->next = stack->top;
  stack->top = new;
}

node_t* pop_token(token_stack_t* stack)
{
  node_t* res = stack->top;
  stack->top = res->next;
  return res;
}

node_t* dequeue_token(token_queue_t* queue)
{
}

void consume(parser_t* parser, token_type_t type, parsing_error_t error)
{
  if (parser->lexer->current.type == type) {
    advance_lexer(parser->lexer, parser->scanner);
    return;
  }
  parser->is_error = true;
  parser->error = error;
}