#include "src/includes/chunk.h"
#include "src/includes/compiler.h"
#include "src/includes/debug.h"
#include "src/includes/lexer.h"
#include "src/includes/parser.h"
#include "src/includes/scanner.h"
#include "src/includes/vm.h"
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
    char command[20];
    sprintf(command, "cat -n %s", argv[1]);
    system(command);
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
  parsing(&parser);
  if (parser.is_error) {
    free_queue(parser.queue);
    free_chunk(&chunk);
    free(source);
    return;
  }
#ifdef DEBUG_PRINT_TOKEN_QUEUE
  print_queue(parser.queue);
#endif
  init_compiler(&compiler, &chunk, parser.queue);
  compilation(&compiler);
  if (compiler.is_error) {
    free_queue(compiler.queue);
    free_chunk(compiler.chunk);
    free(source);
    return;
  }
#ifdef DEBUG_PRINT_BYTECODE
  debug_chunk(&chunk, "byte code");
#endif

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
