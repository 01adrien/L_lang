#include "includes/parser.h"
#include "includes/memory.h"
#include "includes/vm.h"
#include <stdarg.h>
#include <stdio.h>

void expression(parser_t* parser);
void binary(parser_t* parser);
void literal(parser_t* parser);
void grouping(parser_t* parser);
void handle_minus(parser_t* parser);
void unary(parser_t* parser);
void statement(parser_t* parser);
void assignement(parser_t* parser);
void error_parser(char* msg, ...);

int paren_L_count;
int paren_R_count;

void init_parser(
    lexer_t* lexer, scanner_t* scanner, parser_t* parser, token_stack_t* stack,
    token_queue_t* queue
)
{
  parser->lexer = lexer;
  parser->scanner = scanner;
  parser->queue = queue;
  parser->stack = stack;
  parser->is_error = false;
  parser->queue->head = NULL;
  parser->stack->top = NULL;
  paren_L_count = 0;
  paren_R_count = 0;
}

parsing_error_t parse(parser_t* parser)
{

  lexer_t* lexer = parser->lexer;
  scanner_t* scanner = parser->scanner;
  token_stack_t* stack = parser->stack;
  while (lexer->next.type != TOKEN_EOF) {
    advance_lexer(lexer, scanner);
    expression(parser);
    if (parser->is_error) {
      return "error parser";
    }
  }
  while (stack->top) {
    enqueue_token(parser, get_token(pop_token(stack)));
  }
#ifdef DEBUG_PRINT_TOKEN_QUEUE
  print_queue(parser->queue);
#endif
}

void expression(parser_t* parser)
{
  lexer_t* lexer = parser->lexer;
  token_t token = lexer->current;
  switch (token.type) {
  case TOKEN_STRING:
  case TOKEN_TRUE:
  case TOKEN_FALSE:
  case TOKEN_NIL:
  case TOKEN_NUMBER:
  case TOKEN_IDENTIFIER:
    literal(parser);
    break;
  case TOKEN_EQUAL:
  case TOKEN_VAR:
    assignement(parser);
    break;
  case TOKEN_BANG_EQUAL:
  case TOKEN_GREATER:
  case TOKEN_GREATER_EQUAL:
  case TOKEN_LESS:
  case TOKEN_LESS_EQUAL:
  case TOKEN_EQUAL_EQUAL:
  case TOKEN_MODULO:
  case TOKEN_PLUS:
  case TOKEN_STAR:
  case TOKEN_SLASH:
    binary(parser);
    break;
  case TOKEN_MINUS:
    handle_minus(parser);
    break;
  case TOKEN_BANG:
    unary(parser);
    break;
  case TOKEN_SEMICOLON:
    statement(parser);
    break;
  case TOKEN_NEWLINE:
    enqueue_token(parser, token);
    if (lexer->previous.type != TOKEN_SEMICOLON
        && lexer->previous.type != TOKEN_LEFT_BRACE
        && lexer->previous.type != TOKEN_NEWLINE) {
      error_parser("missing ';' after statement at line %d.", token.line);
      parser->is_error = true;
      return;
    }
    if (paren_L_count != paren_R_count) {
      error_parser("unmatched parenthesis at line %d", token.line - 1);
      parser->is_error = true;
      paren_L_count = paren_R_count = 0;
      return;
    }
    break;
  case TOKEN_LEFT_BRACE:
  case TOKEN_RIGHT_BRACE:
  case TOKEN_PRINT:
    enqueue_token(parser, token);
    break;
  case TOKEN_LEFT_PAREN:
    paren_L_count++;
    enqueue_token(parser, token);
    push_token(parser, token);
    break;
  case TOKEN_RIGHT_PAREN:
    paren_R_count++;
    grouping(parser);
    break;
  case TOKEN_EMPTY:
  case TOKEN_EOF:
    return;
  case TOKEN_ERROR:
    error_parser("%s at line %d.", token.start, token.line);
    parser->is_error = true;
    exit(0);
    return;
  default:
    printf("Unknow token : ");
    PRINT_TOKEN(lexer->current);
    break;
  }
}

void binary(parser_t* parser)
{
  token_t token = parser->lexer->current;
  push_token(parser, token);
}

void literal(parser_t* parser)
{
  token_t token = parser->lexer->current;
  enqueue_token(parser, token);
}

void grouping(parser_t* parser)
{
  token_queue_t* queue = parser->queue;
  token_stack_t* stack = parser->stack;
  node_t* node = pop_token(stack);
  while (node && node->token.type != TOKEN_LEFT_PAREN) {
    enqueue_token(parser, get_token(node));
    node = pop_token(stack);
  }
  if (node && node->token.type == TOKEN_LEFT_PAREN) {
    FREE(node, node_t);
  }
  enqueue_token(parser, parser->lexer->current);
}

void unary(parser_t* parser)
{
  token_t token = parser->lexer->current;
  switch (token.type) {
  case TOKEN_BANG:
    push_token(parser, token);
    break;
  case TOKEN_MINUS:
    push_token(
        parser, make_token(TOKEN_MINUS_UNARY, parser->scanner, PREC_UNARY)
    );
    break;
  default:
    return;
  }
}

void handle_minus(parser_t* parser)
{
  token_t token = parser->lexer->previous;
  if (token.type == TOKEN_LEFT_PAREN || token.type == TOKEN_EMPTY
      || token.type == TOKEN_EQUAL) {
    unary(parser);
    return;
  }
  else {
    binary(parser);
  }
}

void statement(parser_t* parser)
{
  token_t token = parser->lexer->current;
  token_queue_t* queue = parser->queue;
  token_stack_t* stack = parser->stack;
  while (stack->top) {
    enqueue_token(parser, get_token(pop_token(stack)));
  }
  enqueue_token(parser, token);
}

void assignement(parser_t* parser)
{
  enqueue_token(parser, parser->lexer->current);
}

void error_parser(char* msg, ...)
{
  va_list args;
  va_start(args, msg);
  va_end(args);
  fprintf(stderr, "Error: ");
  vfprintf(stderr, msg, args);
  printf("\n");
}

//*************** SHUNTING YARD ****************//

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

void enqueue_token(parser_t* parser, token_t token)
{
  token_queue_t* queue = parser->queue;
  token_stack_t* stack = parser->stack;
  node_t* new = create_node(token);
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
  node_t* new = create_node(token);
  if (token.type != TOKEN_LEFT_PAREN && token.type != TOKEN_RIGHT_PAREN
      && stack->top) {
    token_t top_token = stack->top->token;
    while (stack->top && top_token.precedence >= token.precedence) {
      token_t t = get_token(pop_token(stack));
      if (t.type != TOKEN_LEFT_PAREN) {
        enqueue_token(parser, t);
      }
    }
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
  if (!stack->top) {
    return NULL;
  }
  node_t* res = stack->top;
  stack->top = res->next;
  return res;
}

node_t* dequeue_token(token_queue_t* queue)
{
  if (!queue->head) {
    return NULL;
  }
  node_t* res = queue->head;
  queue->head = res->next;
  return res;
}

node_t* peek_queue(token_queue_t* queue)
{
  return queue->head;
}

token_t get_token(node_t* node)
{
  token_t token = node->token;
  FREE(node, node_t);
  return token;
}

node_t* create_node(token_t token)
{
  node_t* node = calloc(1, sizeof(node_t));
  node->token = token;
  node->next = NULL;
  return node;
}

void free_queue(token_queue_t* queue)
{
  node_t* temp = queue->head;
  while (temp) {
    node_t* del = temp;
    temp = temp->next;
    free(del);
  }
}