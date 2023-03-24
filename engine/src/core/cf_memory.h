#ifndef CF_MEMORY_H
#define CF_MEMORY_H

#include "def.h"

CFAPI void* cfAllocate(u64 size);
CFAPI void cfFree(void* block);
CFAPI void* cfCopyMemory(void* src, void* dst, u64 size);
CFAPI void* cfSetMemory(void* block, i32 val, u64 size);
CFAPI void* cfZeroMemory(void* block, u64 size);

#endif