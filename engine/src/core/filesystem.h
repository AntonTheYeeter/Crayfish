#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "def.h"

CFAPI const char* fileReadAll(const char* filename, u64* filesize);

#endif