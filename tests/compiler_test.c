#include "criterion.h"

void setup_compiler(void)
{
}

void teardown_compiler(void)
{
}

TestSuite(compiler_test, .init = setup_compiler, .fini = teardown_compiler);
