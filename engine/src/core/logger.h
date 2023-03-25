#ifndef LOGGER_H
#define LOGGER_H

#include "def.h"

b8 loggerStartup();
void loggerShutdown();

typedef enum LogLevel
{
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE
} LogLevel;

CFAPI void _log(LogLevel lvl, const char* msg, ...);

#define CF_FATAL(msg, ...) _log(LOG_LEVEL_FATAL, msg, ##__VA_ARGS__);
#define CF_ERROR(msg, ...) _log(LOG_LEVEL_ERROR, msg, ##__VA_ARGS__);
#define CF_WARN(msg, ...) _log(LOG_LEVEL_WARN, msg, ##__VA_ARGS__);
#define CF_INFO(msg, ...) _log(LOG_LEVEL_INFO, msg, ##__VA_ARGS__);
#define CF_DEBUG(msg, ...) _log(LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__);
#define CF_TRACE(msg, ...) _log(LOG_LEVEL_TRACE, msg, ##__VA_ARGS__);

#endif