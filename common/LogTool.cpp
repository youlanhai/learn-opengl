#include "LogTool.h"

#include <cstdarg>
#include <ctime>
#include <cassert>
#include <algorithm>

int g_logPriorityFilter = LOG_PRIORITY_DEBUG;

bool g_logToConsole = true;

static const int MaxLogLength = 1024 * 4;

static const char * LogPriorityString[] = {
    "DEBUG",
    "INFO",
    "NOTICE",
    "WARN",
    "ERROR",
    "FATAL"
};

const char* getLogPriorityString(int priority)
{
    return LogPriorityString[std::min(priority, LOG_PRIORITY_FATAL)];
}

void EditorLog(int moduleLvl, int logLvl, const char * tag, const char *format, ...)
{
    assert(logLvl >= 0);
    
    if(logLvl < std::max(moduleLvl, g_logPriorityFilter))
    {
        return;
    }
    
    char message[MaxLogLength];
    int length = 0;
    
    va_list ap;
    va_start(ap, format);
    length = vsnprintf(message, MaxLogLength, format, ap);
    va_end(ap);

    if(length < 0)
    {
        return;
    }
    
    if(g_logToConsole)
    {
        time_t timeVal = time(nullptr);
        struct tm timeinfo = *gmtime(&timeVal);
        const char * prefix = getLogPriorityString(logLvl);
        
        fprintf(stdout, "[%2.2d:%2.2d:%2.2d][%s][%s]", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, prefix, tag);
        fputs(message, stdout);
        fputs("\n", stdout);
        fflush(stdout);
    }
    
    if(logLvl >= LOG_PRIORITY_FATAL)
    {
        abort();
    }
}
