#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "memory.h"
#include "value.h"
#include "vm.h"

void
initChunk(Chunk *chunk)
{
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  chunk->rle_cnt = 0;
  chunk->rle_lines = NULL;
  chunk->rle_cnts = NULL;
  initValueArray(&chunk->constants);
}

void
freeChunk(Chunk *chunk)
{
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  FREE_ARRAY(int, chunk->rle_lines, chunk->capacity);
  FREE_ARRAY(int, chunk->rle_cnts, chunk->capacity);
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}

void
writeChunk(Chunk *chunk, uint8_t byte, int line)
{
  if (chunk->capacity < chunk->count + 1)
  {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code
        = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    chunk->rle_lines
        = GROW_ARRAY(int, chunk->rle_lines, oldCapacity, chunk->capacity);
    chunk->rle_cnts
        = GROW_ARRAY(int, chunk->rle_cnts, oldCapacity, chunk->capacity);
  }

  chunk->code[chunk->count] = byte;
  if (chunk->rle_cnt > 0 && chunk->rle_lines[chunk->rle_cnt - 1] == line)
  { // same line
    chunk->rle_cnts[chunk->rle_cnt - 1]++;
  }
  else
  {
    chunk->rle_lines[chunk->rle_cnt] = line;
    chunk->rle_cnts[chunk->rle_cnt++] = 1; // first line
  }
  // chunk->lines[chunk->count] = line;
  chunk->count++;
}

int
addConstant(Chunk *chunk, Value value)
{
  push(value);
  writeValueArray(&chunk->constants, value);
  pop();
  return chunk->constants.count - 1;
}

void
writeConstant(Chunk *chunk, Value value, int line)
{
  int index = addConstant(chunk, value);
  if (index < 0xff)
  {
    writeChunk(chunk, OP_CONSTANT, line);
    writeChunk(chunk, (uint8_t)index, line);
  }
  else
  {
    writeChunk(chunk, OP_CONSTANT_LONG, line);
    writeChunk(chunk, (uint8_t)(index & 0xff), line);         // low byte
    writeChunk(chunk, (uint8_t)((index >> 8) & 0xff), line);  // mid byte
    writeChunk(chunk, (uint8_t)((index >> 16) & 0xff), line); // high byte
  }
}
