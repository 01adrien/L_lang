#include "includes/scanner.h"

void init_scanner(const char* source, scanner_t* scanner)
{
  scanner->current = source;
  scanner->start = source;
  scanner->line = 1;
}

bool end_file(scanner_t* scanner)
{
  return *(scanner->current) == '\0';
}

char advance_scanner(scanner_t* scanner)
{
  scanner->current++;
  return scanner->current[-1];
}

void skip_blanks(scanner_t* scanner)
{
  // handle comment
  for (;;) {
    char c = peek_scanner(scanner);
    switch (c) {
    case ' ':
    case '\r':
    case '\t':
    case '\n':
      advance_scanner(scanner);
      break;
    default:
      return;
    }
  }
}

char peek_scanner(scanner_t* scanner)
{
  return *(scanner->current);
}

bool match(scanner_t* scanner, char c)
{
  if (*(scanner->current) == c) {
    advance_scanner(scanner);
    return true;
  }
  return false;
}
