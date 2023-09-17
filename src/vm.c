#include "includes/vm.h"
#include "includes/chunk.h"
#include <stdio.h>

void reset_vm(chunk_t* chunk);
void print_vm_stack();
void push(value_t value);
value_t pop();
interpret_result_t binary_operation(char sign);
value_t peek_stack(int n);
interpret_result_t not();

vm_t vm;

interpret_result_t run_vm(chunk_t* chunk)
{
  reset_vm(chunk);
  for (;;) {
#ifdef DEBUG_STACK_TRACE_EXECUTION
    print_VM_stack();
#endif
    OPcode_t instruction = *vm.ip++;
    switch (instruction) {
    case OP_CONSTANT_8: {
      value_t constant = chunk->constants.values[*vm.ip++];
      push(constant);
      break;
    }
    case OP_CONSTANT_16: {
      uint16_t index = ((*vm.ip++) << 8) | ((*vm.ip++) >> 8);
      value_t constant = chunk->constants.values[index];
      push(constant);
      break;
    }
    case OP_ADD: {
      binary_operation('+');
      break;
    }
    case OP_MULTIPLY: {
      binary_operation('*');
      break;
    }
    case OP_SUBSTRACT: {
      binary_operation('-');
      break;
    }
    case OP_DIVIDE: {
      if (peek_stack(0).as_double == 0) {
        return ERROR_DIVISION_BY_ZERO;
      }
      binary_operation('/');
      break;
    }
    case OP_NEGATE: {
      push(NUMBER_VAL(-(AS_NUMBER(pop()))));
      break;
    }
    case OP_MODULO: {
      binary_operation('%');
      break;
    }
    case OP_GREATER: {
      binary_operation('>');
      break;
    }
    case OP_LESS: {
      binary_operation('<');
      break;
    }
    case OP_NOT: {
      not();
      break;
    }
    case OP_EQUAL: {
      // check types
      push(EQ(pop().as_int, pop().as_int) ? TRUE_VAL : FALSE_VAL);
      break;
    }
    case OP_TRUE: {
      push(TRUE_VAL);
      break;
    }
    case OP_FALSE: {
      push(FALSE_VAL);
      break;
    }
    case OP_NIL: {
      push(NIL_VAL);
      break;
    };
    case OP_RETURN: {
      print_value(pop());
      printf("\n");
      return SUCCESS;
    }
    default:
      return ERROR_UNKNOWN_OPCODE;
    }
  }
}

void reset_vm(chunk_t* chunk)
{
  vm.stack_top = vm.stack;
  vm.ip = chunk->OPcodes;
  vm.chunk = chunk;
}

void print_vm_stack()
{
  for (value_t* elmt = vm.stack; elmt < vm.stack_top; elmt++) {
    printf("[ ");
    print_value(*elmt);
    printf(" ]");
  }
  printf("\n");
}

void push(value_t value)
{
  *vm.stack_top = value;
  vm.stack_top++;
}

value_t pop()
{
  vm.stack_top--;
  return *vm.stack_top;
}

interpret_result_t binary_operation(char sign)
{
  if (!IS_NUMBER(peek_stack(0)) || !IS_NUMBER(peek_stack(1))) {
    return ERROR_RUNTIME_EXCEPTION;
  }
  int32_t a = AS_NUMBER(pop());
  int32_t b = AS_NUMBER(pop());
  switch (sign) {
  case '+':
    push(NUMBER_VAL(a + b));
    break;
  case '*':
    push(NUMBER_VAL(a * b));
    break;
  case '-':
    push(NUMBER_VAL(b - a));
    break;
  case '/':
    push(NUMBER_VAL(b / a));
    break;
  case '%':
    push(NUMBER_VAL(b % a));
    break;
  case '>':
    push(b > a ? TRUE_VAL : FALSE_VAL);
    break;
  case '<':
    push(b < a ? TRUE_VAL : FALSE_VAL);
    break;
  default:
    return ERROR_RUNTIME_EXCEPTION;
  }
}

interpret_result_t not()
{

  if (!IS_BOOLEAN(peek_stack(0))) {
    return ERROR_RUNTIME_EXCEPTION;
  }
  push(AS_BOOLEAN(pop()) == 1 ? FALSE_VAL : TRUE_VAL);
}

value_t peek_stack(int n)
{
  return vm.stack_top[-1 - n];
}

char* interpreter_status(interpret_result_t status)
{
  switch (status) {
  case SUCCESS:
    return "SUCCES";
  case ERROR_DIVISION_BY_ZERO:
    return "DIVISION BY ZERO ERROR";
  case ERROR_RUNTIME_EXCEPTION:
    return "RUNTIME EXCEPTION ERROR";
  case ERROR_UNKNOWN_OPCODE:
    return "UNKNOWN OPCODE ERROR";
  case ERROR_END_OF_STREAM:
    return "END OF STREAM ERROR";
  case ERROR_STACK_OVERFLOW:
    return "STACK OVERFLOW ERROR";
  default:
    return "UNKNOW STATUS CODE";
  }
};
