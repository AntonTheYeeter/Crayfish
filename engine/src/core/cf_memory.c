#include "cf_memory.h"

#include <stdlib.h>
#include <string.h>

void* cfAllocate(u64 size)
{
    return malloc(size);
}

void cfFree(void* block)
{
    free(block);
}

void* cfCopyMemory(void* src, void* dst, u64 size)
{
    return memcpy(dst, src, size);
}

void* cfSetMemory(void* block, i32 val, u64 size)
{
    return memset(block, val, size);
}

void* cfZeroMemory(void* block, u64 size)
{
    return memset(block, 0, size);
}