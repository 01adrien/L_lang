#include "../src/includes/scanner.h"
#include "criterion.h"

scanner_t scanner;

void setup_scanner(void)
{
}

void teardown_scanner(void)
{
}

TestSuite(scanner_test, .init = setup_scanner, .fini = teardown_scanner);

Test(scanner_test, scan_string)
{
  const char* str = "test string";
  init_scanner(str, &scanner);
  cr_assert_str_eq(scanner.current, "test string");
  cr_assert_str_eq(scanner.start, "test string");
  const char c = advance_scanner(&scanner);
  cr_assert_eq(c, 't');
}

Test(scanner_test, end_of_file)
{
  const char* str = "abc";
  init_scanner(str, &scanner);
  advance_scanner(&scanner);
  advance_scanner(&scanner);
  advance_scanner(&scanner);
  cr_assert(end_file(&scanner));
}
