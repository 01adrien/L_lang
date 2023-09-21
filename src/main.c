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
  vm.objects = NULL;
  while (lexer.next.type != TOKEN_EOF && !parser.is_error && !parse_err) {
    parse_err = parse(&parser);
  }
  if (parse_err || parser.is_error) {
    printf("%s\n", parse_err ? parse_err : parser.error);
    free_chunk(parser.chunk);
    return;
  }
  generate_btc(&parser);
  emit_byte(parser.chunk, OP_RETURN);
#ifdef DEBUG_PRINT_BYTECODE
  debug_chunk(parser.chunk, "byte code");
  printf("\n");
#endif
  printf("\n");
  interpret_result_t res = run_vm(parser.chunk);
  printf("\n%s\n\n", interpreter_status(res));
  free_chunk(parser.chunk);
  free(source);
  free_vm();
}

char* read_file(const char* path)
{
  FILE* file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }
  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);
  char* buffer = (char*) malloc(fileSize + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  if (bytesRead < fileSize) {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }

  buffer[bytesRead] = '\0';
  fclose(file);
  return buffer;
}
