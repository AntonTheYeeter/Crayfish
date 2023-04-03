#include "filesystem.h"

#include <stdio.h>

#include "core/cf_memory.h"
#include "core/logger.h"

const char* fileReadAll(const char* filename, u64* filesize)
{
    FILE* f = fopen(filename, "rb");

    if(!f)
    {
        CF_ERROR("Failed to open file %s", filename);
        return PNULL;
    }

    fseek(f, 0, SEEK_END);
    u64 size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = cfAllocate(size);
    cfZeroMemory(buffer, size);
    fread(buffer, 1, size, f);

    *filesize = size;
    return buffer;
}