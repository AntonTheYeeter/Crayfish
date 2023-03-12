#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

b8 loggerStartup()
{

}

void loggerShutdown()
{

}

CFAPI void _write(LogLevel lvl, const char* msg, ...)
{
    const char* prefix[] = 
    {
        "[FATAL]: ",
        "[ERROR]: ",
        "[WARN]: ",
        "[INFO]: ",
        "[DEBUG]: ",
        "[TRACE]: "
    };

    char buffer[32000];
    memset(buffer, 0, 32000);

    __builtin_va_list arg;
    va_start(arg, msg);
    vsnprintf(buffer, 32000, msg, arg);
    va_end(arg);

    const char* colorCodes[] =
    {
        "[41;30m",
        "[0;31m",
        "[0;33m",
        "[0;37m",
        "[0;34m",
        "[0;32m"
    };

    printf("\e%s%s%s\e[0m\n", colorCodes[lvl], prefix[lvl], buffer);
}