#include "logger.h"

#ifdef _MSC_VER
#define debugBreak __debugbreak
#else
#define debugBreak __builtin_trap
#endif

#define CF_ASSERT(val)                                                                      \
{                                                                                           \
    if(val){}                                                                               \
    else                                                                                    \
    {                                                                                       \
        _log(LOG_LEVEL_FATAL, "Assertion happened at %s line %i!", __FILE__, __LINE__);     \
        debugBreak();                                                                       \
    }                                                                                       \
}

#define CF_ASSERT_MSG(val, msg)                                                                                 \
{                                                                                                               \
    if(val){}                                                                                                   \
    else                                                                                                        \
    {                                                                                                           \
        _log(LOG_LEVEL_FATAL, "Assertion happened at %s line %i: %s!", __FILE__, __LINE__, msg);                \
        debugBreak();                                                                                           \
    }                                                                                                           \
}