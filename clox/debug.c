#include <stdint.h>
#include <stdio.h>

#include "chunk.h"
#include "debug.h"
#include "value.h"

static int
simpleInstruction(const char *name, int offset)
{
  printf("%s\n", name);
  return offset + 1;
}

static int
constantInstruction(const char *name, Chunk *chunk, int offset)
{
  uint8_t constant = chunk->code[offset + 1];
  printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 2;
}

static int
constantLongInstruction(const char *name, Chunk *chunk, int offset)
{
  uint32_t constant = chunk->code[offset + 3] << 16
                      | chunk->code[offset + 2] << 8 | chunk->code[offset + 1];
  printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 4;
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

void
disassembleChunk(Chunk *chunk, const char *name)
{
  printf("== %s ==\n", name);

  for (int offset = 0; offset < chunk->count;)
  {
    offset = disassembleInstruction(chunk, offset);
  }
}

int
disassembleInstruction(Chunk *chunk, int offset)
{
  printf("%04d ", offset);

  // Print the line number only if it's the start of a new line.
  if (offset > 0 && getLine(chunk, offset) == getLine(chunk, offset - 1))
  {
    printf("   | ");
  }
  else
  {
    printf("%4d ", getLine(chunk, offset));
  }

  uint8_t instruction = chunk->code[offset];
  switch (instruction)
  {
  case OP_RETURN:
    return simpleInstruction("OP_RETURN", offset);
  case OP_CONSTANT:
    return constantInstruction("OP_CONSTANT", chunk, offset);
  case OP_CONSTANT_LONG:
    return constantLongInstruction("OP_CONSTANT_LONG", chunk, offset);
  case OP_NIL:
    return simpleInstruction("OP_NIL", offset);
  case OP_TRUE:
    return simpleInstruction("OP_TRUE", offset);
  case OP_FALSE:
    return simpleInstruction("OP_FALSE", offset);
  case OP_NEGATE:
    return simpleInstruction("OP_NEGATE", offset);
  case OP_EQUAL:
    return simpleInstruction("OP_EQUAL", offset);
  case OP_GREATER:
    return simpleInstruction("OP_GREATER", offset);
  case OP_LESS:
    return simpleInstruction("OP_LESS", offset);
  case OP_ADD:
    return simpleInstruction("OP_ADD", offset);
  case OP_SUBTRACT:
    return simpleInstruction("OP_SUBTRACT", offset);
  case OP_MULTIPLY:
    return simpleInstruction("OP_MULTIPLY", offset);
  case OP_DIVIDE:
    return simpleInstruction("OP_DIVIDE", offset);
  case OP_NOT:
    return simpleInstruction("OP_NOT", offset);
  case OP_PRINT:
    return simpleInstruction("OP_PRINT", offset);
  case OP_POP:
    return simpleInstruction("OP_POP", offset);
  case OP_GET_GLOBAL:
    return constantInstruction("OP_GET_GLOBAL", chunk, offset);
  case OP_DEFINE_GLOBAL:
    return constantInstruction("OP_DEFINE_GLOBAL", chunk, offset);
  case OP_SET_GLOBAL:
    return constantInstruction("OP_SET_GLOBAL", chunk, offset);
  default:
    printf("Unknown opcode %d\n", instruction);
    return offset + 1;
  }
}
