#include "vm.h"
#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

VM vm;

static Value
peek(int distance)
{
  return vm.stackTop[-1 - distance];
}

static bool
isFalsey(Value value)
{
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void
concatenate()
{
  ObjString *b = AS_STRING(pop());
  ObjString *a = AS_STRING(pop());

  int length = a->length + b->length;
  char *chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  ObjString *result = takeString(chars, length);
  push(OBJ_VAL(result));
}

static void
resetStack()
{
  vm.stackTop = vm.stack;
}

static int
getLine(Chunk *chunk, int instructionIndex)
{
  int line = 0;
  int currentInstructionIndex = 0; // 当前指令索引
  int rleIndex = 0;                // RLE编码块的索引

  // 遍历指令并查找所在的RLE编码块
  while (rleIndex < chunk->rle_cnt)
  {
    int rleLine = chunk->rle_lines[rleIndex];
    int rleCount = chunk->rle_cnts[rleIndex];

    // 如果当前指令索引在这个RLE编码块内，找到了对应行号
    if (currentInstructionIndex + rleCount > instructionIndex)
    {
      line = rleLine;
      break;
    }
    else
    {
      // 否则，继续前进到下一个RLE编码块
      currentInstructionIndex += rleCount;
      rleIndex++;
    }
  }

  return line;
}

static void
runtimeError(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  size_t instruction = vm.ip - vm.chunk->code - 1;
  int line = getLine(vm.chunk, instruction);
  fprintf(stderr, "[line %d] in script\n", line);
  resetStack();
}

static InterpretResult
run()
{
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(valueType, op)                                              \
  do                                                                          \
  {                                                                           \
    if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1)))                           \
    {                                                                         \
      runtimeError("Operands must be numbers.");                              \
      return INTERPRET_RUNTIME_ERROR;                                         \
    }                                                                         \
    double b = AS_NUMBER(pop());                                              \
    double a = AS_NUMBER(pop());                                              \
    push(valueType(a op b));                                                  \
  } while (false)
  for (;;)
  {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (Value *slot = vm.stack; slot < vm.stackTop; slot++)
    {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");
    disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
    uint8_t instruction;
    switch (instruction = READ_BYTE())
    {
    case OP_RETURN:
    {
      printValue(pop());
      printf("\n");
      return INTERPRET_OK;
    }
    case OP_CONSTANT:
    {
      Value constant = vm.chunk->constants.values[READ_BYTE()];
      push(constant);
      break;
    }
    case OP_CONSTANT_LONG:
    {
      uint8_t lowbyte = READ_BYTE();
      uint8_t midbyte = READ_BYTE();
      uint8_t highbyte = READ_BYTE();
      uint32_t index = (highbyte << 16) | (midbyte << 8) | lowbyte;
      push(vm.chunk->constants.values[index]);
      break;
    }
    case OP_NIL:
      push(NIL_VAL);
      break;
    case OP_TRUE:
      push(BOOL_VAL(true));
      break;
    case OP_FALSE:
      push(BOOL_VAL(false));
      break;
    case OP_EQUAL:
    {
      Value b = pop();
      Value a = pop();
      push(BOOL_VAL(valuesEqual(a, b)));
      break;
    }
    case OP_GREATER:
      BINARY_OP(BOOL_VAL, >);
      break;
    case OP_LESS:
      BINARY_OP(BOOL_VAL, <);
      break;
    case OP_ADD:
    {
      if (IS_STRING(peek(0)) && IS_STRING(peek(1)))
      {
        concatenate();
      }
      else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1)))
      {
        double b = AS_NUMBER(pop());
        double a = AS_NUMBER(pop());
        push(NUMBER_VAL(a + b));
      }
      else
      {
        runtimeError("Operands must be two numbers or two strings.");
        return INTERPRET_RUNTIME_ERROR;
      }
      break;
    }
    case OP_SUBTRACT:
      BINARY_OP(NUMBER_VAL, -);
      break;
    case OP_MULTIPLY:
      BINARY_OP(NUMBER_VAL, *);
      break;
    case OP_DIVIDE:
      BINARY_OP(NUMBER_VAL, /);
      break;
    case OP_NOT:
      push(BOOL_VAL(isFalsey(pop())));
      break;
    case OP_NEGATE:
    {
      if (!IS_NUMBER(peek(0)))
      {
        runtimeError("Operand must be a number.");
        return INTERPRET_RUNTIME_ERROR;
      }
      break;
    }
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
    }
  }
}

void
initVM()
{
  resetStack();
  vm.objects = NULL;
}

void
freeVM()
{
  freeObjects();
}

InterpretResult
interpret(const char *source)
{
  Chunk chunk;
  initChunk(&chunk);

  if (!compile(source, &chunk))
  {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }

  vm.chunk = &chunk;
  vm.ip = vm.chunk->code;

  InterpretResult result = run();

  freeChunk(&chunk);
  return result;
}

void
push(Value value)
{
  *vm.stackTop = value;
  vm.stackTop++;
}

Value
pop()
{
  vm.stackTop--;
  return *vm.stackTop;
}