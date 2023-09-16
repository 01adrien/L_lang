#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "chunk.h"
#include "common.h"

void debug_chunk(chunk_t* chunk, const char* name);
int debug_instruction(chunk_t* chunk, int offset);

#endif   // !