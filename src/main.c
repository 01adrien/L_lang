#include "includes/chunk.h"
#include "includes/debug.h"
#include "includes/lexer.h"
#include "includes/parser.h"
#include "includes/scanner.h"
#include "includes/vm.h"
#include <stdio.h>

void repl();
void run_file(const char* path);
char* read_file(const char* path);

scanner_t scanner;
lexer_t lexer;
parser_t parser;
chunk_t chunk;
token_queue_t token_queue;
token_stack_t token_stack;

int main(int argc, char const* argv[])
{
  if (argc == 2) {
    run_file(argv[1]);
  }
  if (argc == 1) {
    repl();
  }
}

void repl()
{
  printf("repl\n");
}

void run_file(const char* path)
{
  char* source = read_file(path);
  init_scanner(source, &scanner);
  init_parser(&lexer, &scanner, &parser, &chunk, &token_stack, &token_queue);
  advance_lexer(&lexer, &scanner);
  parsing_error_t parse_err = NULL;
  while (lexer.current.type != TOKEN_EOF || !parser.is_error || !parse_err) {
    parse_err = parse(&parser, PREC_ASSIGNMENT);
  }

  printf("Q\n");
  print_queue(parser.queue);
  printf("S\n");
  print_stack(parser.stack);

  /*
  if (parse_err || parser.is_error) {
    printf("%s\n", parse_err ? parse_err : parser.error);
    free_chunk(&chunk);
    return;
  }
  emit_byte(&chunk, OP_RETURN);

#ifdef DEBUG_PRINT_BYTECODE
  debug_chunk(&chunk, "byte code");
#endif

  interpret_result_t interpret_err = run_vm(&chunk);
  printf("interpreter return status: %s\n", interpreter_status(interpret_err));
  free_chunk(&chunk);
  */
}

char* read_file(const char* path)
{
  FILE* file = fopen(path, "rb");
  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);
  char* buffer = (char*) malloc(fileSize + 1);
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  buffer[bytesRead] = '\0';
  fclose(file);
  return buffer;
}
