#include "log_query.h"

namespace LOG {
LogQuery MakeLogQuery(std::string name, Log_type type, const char *file, const int fileLine, const char *format, ...) {
    LogQuery query;
    query.name = name;
    query.type = type;

    //int n_args = GetNumberOfArguments(format);
    va_list args;
    va_start(args, format);

    char buffer[512];
    sprintf(buffer, " [%s:%d] ", file, fileLine);

    query.log = buffer;

    vsprintf(buffer, format, args);
    query.log += buffer;

    return query;
}

int GetNumberOfArguments(const char *format) {
    int i = 0, ret = 0;

    while (format[i] != 0) {
        if (format[i] == '%')
            ret++;

        i++;
    }

    return ret;
}
}
