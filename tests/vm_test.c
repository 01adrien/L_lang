#include "criterion.h"

void setup_vm(void)
{
}

void teardown_vm(void)
{
}

TestSuite(vm_test, .init = setup_vm, .fini = teardown_vm);
