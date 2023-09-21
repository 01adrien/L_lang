#include "includes/parser.h"
#include "includes/vm.h"
#include <stdio.h>

// Parsing rules
void expression(parser_t* parser);
void binary(parser_t* parser);
void literal(parser_t* parser);
void grouping(parser_t* parser);
void handle_minus(parser_t* parser);
void unary(parser_t* parser);
void declaration(parser_t* parser);
void statement(parser_t* parser);
void assignement(parser_t* parser);

// Helpers
void consume(parser_t* parser, token_type_t type, parsing_error_t error);
object_t* string(token_t token);
uint16_t get_constant_var(chunk_t* chunk, token_t token);
bool match_token(parser_t* parser, token_type_t type);
void generate_one_btc(parser_t* parser);

// Shunting yard
void print_queue(token_queue_t* queue);
void print_stack(token_stack_t* stack);
void enqueue_token(parser_t* parser, token_t token);
void push_token(parser_t* parser, token_t token);
node_t* dequeue_token(token_queue_t* queue);
node_t* pop_token(token_stack_t* stack);
node_t* create_node(token_t token);
token_t get_token(node_t* node);
node_t* peek_queue(token_queue_t* queue);

void init_parser(
    lexer_t* lexer, scanner_t* scanner, parser_t* parser, chunk_t* chunk,
    token_stack_t* stack, token_queue_t* queue
)
{
  parser->lexer = lexer;
  parser->scanner = scanner;
  parser->chunk = chunk;
  parser->queue = queue;
  parser->stack = stack;
  parser->is_error = false;
  parser->queue->head = NULL;
  parser->error = NULL;
  parser->stack->top = NULL;
}

parsing_error_t parse(parser_t* parser)
{

  lexer_t* lexer = parser->lexer;
  scanner_t* scanner = parser->scanner;
  token_stack_t* stack = parser->stack;
  while (lexer->next.type != TOKEN_EOF) {
    advance_lexer(lexer, scanner);
    expression(parser);
  }
  while (stack->top) {
    enqueue_token(parser, get_token(pop_token(stack)));
  }
#ifdef DEBUG_PRINT_QUEUE
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
  case TOKEN_PRINT:
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
  case TOKEN_LEFT_PAREN:
    push_token(parser, token);
    break;
  case TOKEN_RIGHT_PAREN:
    grouping(parser);
    break;
  case TOKEN_EMPTY:
  case TOKEN_EOF:
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
  if (token.type == TOKEN_LEFT_PAREN || token.type == TOKEN_EMPTY) {
    unary(parser);
    return;
  }
  else {
    binary(parser);
  }
}

void declaration(parser_t* parser)
{
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

//**************** HELPERS ********************//

void consume(parser_t* parser, token_type_t type, parsing_error_t error)
{
  token_t t = parser->lexer->current;
  if (parser->lexer->next.type == type) {
    advance_lexer(parser->lexer, parser->scanner);
    return;
  }

  // parser->is_error = true;
  // parser->error = error;
}
bool match_token(parser_t* parser, token_type_t type)
{
  if (parser->lexer->current.type == type) {
    advance_lexer(parser->lexer, parser->scanner);
    return true;
  }
  return false;
}

uint16_t get_constant_var(chunk_t* chunk, token_t token)
{
  object_t* str = allocate_string(token.start, token.length);
  return write_constant(chunk, str->value);
}

object_t* string(token_t token)
{
  object_t* str = allocate_string(token.start + 1, token.length - 2);
  return str;
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
  free(node);
  return token;
}

node_t* create_node(token_t token)
{
  node_t* node = calloc(1, sizeof(node_t));
  node->token = token;
  node->next = NULL;
  return node;
}

void generate_btc(parser_t* parser)
{
  token_queue_t* queue = parser->queue;

  while (queue->head) {
    generate_one_btc(parser);
  }
}

void generate_one_btc(parser_t* parser)
{
  chunk_t* chunk = parser->chunk;
  token_queue_t* queue = parser->queue;
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
      emit_constant(chunk, string(token)->value);
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
      while (parser->queue->head->token.type != TOKEN_SEMICOLON) {
        generate_one_btc(parser);
      }
      emit_byte(chunk, OP_PRINT);
      break;
    }
    case TOKEN_VAR: {
      token_t name = get_token(dequeue_token(queue));
      uint16_t index = get_constant_var(parser->chunk, name);
      token_t next = get_token(dequeue_token(queue));
      if (next.type == TOKEN_EQUAL) {
        while (parser->queue->head->token.type != TOKEN_SEMICOLON) {
          generate_one_btc(parser);
        }
        emit_bytes(parser->chunk, OP_DEFINE_GLOBAL, index);
      }
      else {
        emit_byte(parser->chunk, OP_NIL);
      }
      break;
    }
    case TOKEN_IDENTIFIER: {
      uint16_t index = get_constant_var(parser->chunk, token);
      emit_bytes(chunk, OP_GET_GLOBAL, index);
      break;
    }
    default:
      return;
    }
  }
}