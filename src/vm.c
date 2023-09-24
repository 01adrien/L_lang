#include "includes/vm.h"
#include "includes/chunk.h"
#include "includes/memory.h"
#include <stdio.h>

void reset_vm(chunk_t* chunk);
void print_vm_stack();
void push(value_t value);
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
        push(concatenate_string(AS_STRING(pop()), AS_STRING(pop())));
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
      push(values_equal(pop(), pop()) ? TRUE_VAL : FALSE_VAL);
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
    case OP_PRINT: {
      print_value(pop());
      printf("\n");
      break;
    }
    case OP_POP: {
      pop();
      break;
    }
    case OP_DEFINE_GLOBAL: {
      value_t value = pop();
      const char* name = AS_STRING(chunk->constants.values[*vm.ip++]);
      table_set(&vm.globals, name, value);
      break;
    }
    case OP_GET_GLOBAL: {
      const char* name = AS_STRING(chunk->constants.values[*vm.ip++]);
      value_t var = table_get(&vm.globals, name);
      if (IS_NIL(var)) {
        printf("Error: unbound variable '%s'\n", name);
        return ERROR_RUNTIME_EXCEPTION;
      }
      push(var);
      break;
    }
    case OP_SET_GLOBAL: {
      char* name = AS_STRING(chunk->constants.values[*vm.ip++]);
      value_t var = pop();
      table_set(&vm.globals, name, var);
      break;
    }
    case OP_GET_LOCAL: {
      uint8_t slot = *vm.ip++;
      push(vm.stack[slot]);
      break;
    }
    case OP_SET_LOCAL: {
      uint8_t slot = *vm.ip++;
      vm.stack[slot] = peek_stack(0);
      break;
    }
    case OP_RETURN: {
      return SUCCESS;
    }

    default:
      printf("=> %d\n", instruction);
      return ERROR_UNKNOWN_OPCODE;
    }
  }
  return ERROR_END_OF_STREAM;
}

void reset_vm(chunk_t* chunk)
{
  vm.stack_top = vm.stack;
  vm.ip = chunk->OPcodes;
  vm.chunk = chunk;
}

void init_vm()
{
  table_init(&vm.globals);
  table_init(&vm.strings);
  vm.objects = NULL;
}

void free_vm()
{
  table_free(&vm.globals);
  table_free(&vm.strings);
  free_objects();
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

value_t peek_stack(int n)
{
  return vm.stack_top[-1 - n];
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
