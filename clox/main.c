#include "chunk.h"
#include "common.h"
#include "debug.h"

int
main(int argc, char *argv[])
{
  Chunk chunk;
  initChunk(&chunk);
  // int constant = addConstant(&chunk, 1.2);
  for (int i = 0; i < 300; i++)
  {
    writeConstant(&chunk, 1.2, i);
  }
  /** writeChunk(&chunk, OP_CONSTANT, 123); */
  /** writeChunk(&chunk, constant, 123); */

  writeChunk(&chunk, OP_RETURN, 123);
  // printf("%d\n", chunk.rle_cnts[0]);

  disassembleChunk(&chunk, "test chunk");
  freeChunk(&chunk);
  return 0;
}
