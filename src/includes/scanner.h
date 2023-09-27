#ifndef _SCANNER_H_
#define _SCANNER_H_

#include "common.h"

typedef struct scanner {
  const char* start;
  const char* current;
  int line;
} scanner_t;

void init_scanner(const char* source, scanner_t* scanner);
bool end_file(scanner_t* scanner);
char advance_scanner(scanner_t* scanner);
void skip_blanks(scanner_t* scanner);
char peek_scanner(scanner_t* scanner);
bool match(scanner_t* scanner, char c);

#endif   // !