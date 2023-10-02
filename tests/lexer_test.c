#include "../src/includes/lexer.h"
#include "criterion.h"

scanner_t scanner;

void setup_lexer(void)
{
}

void teardown_lexer(void)
{
}

TestSuite(lexer_test, .init = setup_lexer, .fini = teardown_lexer);

Test(lexer_test, tokenize_test)
{
  const char* src = "1 + 1";
  init_scanner(src, &scanner);
  token_t token = scan_token(&scanner);
  cr_assert_eq(token.type, TOKEN_NUMBER);
  token = scan_token(&scanner);
  cr_assert_eq(token.type, TOKEN_PLUS);
  token = scan_token(&scanner);
  cr_assert_eq(token.type, TOKEN_NUMBER);
}

Test(lexer_test, end_of_file_test)
{
  const char* src = "";
  init_scanner(src, &scanner);
  token_t token = scan_token(&scanner);
  cr_assert_eq(token.type, TOKEN_EOF);
  token = scan_token(&scanner);
  cr_assert_eq(token.type, TOKEN_EOF);
}

Test(lexer_token, types_of_token_test)
{
  const char* src = "false == 10 \"hello\" <= # amount";
  init_scanner(src, &scanner);
  token_t token = scan_token(&scanner);
  cr_assert_eq(token.type, TOKEN_FALSE);
  token = scan_token(&scanner);
  cr_assert_eq(token.type, TOKEN_EQUAL_EQUAL);
  token = scan_token(&scanner);
  cr_assert_eq(token.type, TOKEN_NUMBER);
  token = scan_token(&scanner);
  cr_assert_eq(token.type, TOKEN_STRING);
  token = scan_token(&scanner);
  cr_assert_eq(token.type, TOKEN_LESS_EQUAL);
  token = scan_token(&scanner);
  cr_assert_eq(token.type, TOKEN_ERROR);
  token = scan_token(&scanner);
  cr_assert_eq(token.type, TOKEN_IDENTIFIER);
}