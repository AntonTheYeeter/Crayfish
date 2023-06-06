#include "logger.h"

#include <stdarg.h>
#include <stdio.h>

#include "core/cf_memory.h"
#include "platform/platform.h"

b8 loggerStartup()
{
    // For file logging
    return TRUE;
}

void loggerShutdown()
{

}

void _log(LogLevel lvl, const char* msg, ...)
{
    const char* prefix[] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};

    char buffer[32000];
    cfZeroMemory(buffer, 32000);

    va_list arg;
    va_start(arg, msg);
    vsnprintf(buffer, 32000, msg, arg);
    va_end(arg);

    char outMessage[32000];
    cfZeroMemory(outMessage, 32000);

    snprintf(outMessage, 32000, "%s%s\n", prefix[lvl], buffer);
    
    platformWriteConsole(lvl, outMessage);
}