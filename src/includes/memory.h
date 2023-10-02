#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "common.h"

#define GROW_ARRAY_CAPACITY(c) (c < 8 ? 8 : c * 2)

#define GROW_ARRAY_MEMORY(type, pointer, oldCount, newCount)                   \
  (type*) reallocate(                                                          \
      pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount)            \
  )

#define FREE_ARRAY(type, pointer, oldCount)                                    \
  reallocate(pointer, sizeof(type) * (oldCount), 0)

#define FREE(pointer, type) reallocate(pointer, sizeof(type), 0)

#define ALLOCATE(type, count)                                                  \
  (type*) reallocate(NULL, 0, sizeof(type) * (count))

void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#endif   // !