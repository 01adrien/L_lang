#include "includes/vm.h"
#include "includes/chunk.h"
#include <stdio.h>

void reset_vm(chunk_t* chunk);
void print_vm_stack();
void push(value_t value);
bool equality();
value_t concatenate();
value_t pop();
interpret_result_t binary_operation(char sign);
value_t peek_stack(int n);

vm_t vm;

interpret_result_t run_vm(chunk_t* chunk)
{
  reset_vm(chunk);
  for (;;) {
#ifdef DEBUG_STACK_TRACE_EXECUTION
    print_vm_stack();
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
      if (IS_STRING(peek_stack(0)) && IS_STRING(peek_stack(1))) {
        push(concatenate());
      }
      else if (IS_NUMBER(peek_stack(0)) && IS_NUMBER(peek_stack(1))) {
        binary_operation('+');
      }
      else {
        return ERROR_RUNTIME_EXCEPTION;
      }
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
      if (!IS_BOOLEAN(peek_stack(0))) {
        return ERROR_RUNTIME_EXCEPTION;
      }
      push(AS_BOOLEAN(pop()) == 1 ? FALSE_VAL : TRUE_VAL);
      break;
    }
    case OP_EQUAL: {
      push(equality() ? TRUE_VAL : FALSE_VAL);
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
      printf("result: ");
      value_t res = pop();
      print_value(res);
      printf("\n");
      return SUCCESS;
    }
    default:
      printf("=> %d\n", instruction);
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

bool equality()
{
  value_t v1 = pop();
  value_t v2 = pop();
  if (type_of(v1) != type_of(v2)) {
    return false;
  }
  switch (type_of(v1)) {
  case NUMBER_T:
    return AS_NUMBER(v1) == AS_NUMBER(v2);
  case NIL_T:
    return true;
  case BOOLEAN_T:
    return AS_BOOLEAN(v1) == AS_BOOLEAN(v2);
  case STRING_T: {
    char* s1 = AS_STRING(v1);
    char* s2 = AS_STRING(v2);
    return strlen(s1) == strlen(s2) && memcmp(s1, s2, strlen(s1)) == 0;
  }
  default:
    return false;
  }
}

value_t concatenate()
{
  char* s1 = AS_STRING(pop());
  char* s2 = AS_STRING(pop());
  int len = strlen(s1) + strlen(s2) + 1;
  char* new = calloc(len, sizeof(char));
  memcpy(new, s2, strlen(s2));
  memcpy(new + strlen(s2), s1, strlen(s1));
  new[len] = '\0';
  free(s1);
  free(s2);
  return STRING_VAL(new);
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
