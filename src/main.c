#include "includes/chunk.h"
#include "includes/compiler.h"
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
compiler_t compiler;

int main(int argc, char const* argv[])
{
  init_vm();
  if (argc == 2) {
    run_file(argv[1]);
  }
  if (argc == 1) {
    repl();
  }
  free_vm();
}

void repl()
{
  printf("repl\n");
}

void run_file(const char* path)
{
  char* source = read_file(path);
  init_scanner(source, &scanner);
  init_parser(&lexer, &scanner, &parser, &token_stack, &token_queue);
  advance_lexer(&lexer, &scanner);
  parsing_error_t err1 = NULL;
  while (lexer.next.type != TOKEN_EOF) {
    err1 = parse(&parser);
  }
  if (parser.is_error) {
    free_queue(parser.queue);
    free_chunk(&chunk);
    free(source);
    free(parser.error);
    return;
  }
  init_compiler(&compiler, &chunk, parser.queue);
  compile_error_t err2 = NULL;
  err2 = compilation(&compiler);

#ifdef DEBUG_PRINT_BYTECODE
  printf("\n");
  debug_chunk(&chunk, "byte code");
#endif
  /*
   */
  printf("\n");
  interpret_result_t res = run_vm(&chunk);
  printf("\n%s\n\n", interpreter_status(res));
  free_chunk(&chunk);
  free(source);
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
