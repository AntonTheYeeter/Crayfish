#ifndef ASSERTION_H
#define ASSERTION_H

#include "logger.h"

#define CF_ASSERT(val)                                                          \
{                                                                               \
    if(val){}                                                                   \
    else                                                                        \
    {                                                                           \
        CF_FATAL("Assertion happened at %s line %i", __FILE__, __LINE__);       \
    }                                                                           \
}

#define CF_ASSERT_MSG(val, msg, ...)                                                                    \
{                                                                                                       \
    if(val){}                                                                                           \
    else                                                                                                \
    {                                                                                                   \
        CF_FATAL("Assertion happened at %s line %i: %s", __FILE__, __LINE__, msg, ##__VA_ARGS__);       \
    }                                                                                                   \
}

#endif