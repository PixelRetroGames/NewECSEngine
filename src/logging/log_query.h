#pragma once

#include <cstdio>
#include <string>
#include <cstdarg>

namespace LOG {

enum Log_type {
    LT_INFO,
    LT_WARNING,
    LT_ERROR,
    LT_CRITICAL
};

struct LogQuery {
    std::string name;
    Log_type type;
    int color;
    std::string message;
    std::string log;
};

LogQuery MakeLogQuery(std::string name, Log_type type, const char *file, const int fileLine, const char *format, ...);

int GetNumberOfArguments(const char *format);

}
