#include "specialized_logger.h"

namespace LOG {
const int INFO_COLOR = TEXT_WHITE | TEXT_INTENSITY, WARNING_COLOR = TEXT_YELLOW | TEXT_INTENSITY, ERROR_COLOR = TEXT_RED | TEXT_INTENSITY, CRITICAL_COLOR = BG_RED | BG_INTENSITY;

const int SpecializedLogger::name_colors[NUMBER_OF_NAME_COLORS] = {TEXT_GREEN, TEXT_MAGENTA, TEXT_CYAN, TEXT_BLUE, BG_GREEN, BG_MAGENTA, BG_CYAN, BG_BLUE, BG_INTENSITY | TEXT_INTENSITY | TEXT_GREEN,
                                                                   BG_INTENSITY | TEXT_INTENSITY | TEXT_MAGENTA, BG_INTENSITY | TEXT_INTENSITY | TEXT_CYAN, BG_INTENSITY | TEXT_INTENSITY | TEXT_BLUE,
                                                                   TEXT_INTENSITY | TEXT_GREEN, TEXT_INTENSITY | TEXT_MAGENTA, TEXT_INTENSITY | TEXT_CYAN, TEXT_INTENSITY | TEXT_BLUE
                                                                  };

int SpecializedLogger::longest_name_length = 0;

void SpecializedLogger::Enable() {
    enabled = true;
}

void SpecializedLogger::Disable() {
    enabled = false;
}

void SpecializedLogger::Log(LogQuery *query) {
    if (!enabled)
        return;

    std::string message;

    switch (query->type) {
    case LT_INFO:
        query->color = INFO_COLOR;
        query->message = "info";
        break;

    case LT_WARNING:
        query->color = WARNING_COLOR;
        query->message = "warning";
        break;

    case LT_ERROR:
        query->color = ERROR_COLOR;
        query->message = "error";
        break;

    case LT_CRITICAL:
        query->color = CRITICAL_COLOR;
        query->message = "critical";
        break;

    }

    Print_query(query);
}

void SpecializedLogger::Reset_longest_name_length() {
    longest_name_length = 0;
}

void SpecializedLogger::Print_query(LogQuery *query) {
    int col = query->color;
    std::string message = query->message;
    std::string log = query->log;

    char timestamp[100];
    Get_current_time(timestamp);

    const int MESSAGE_LONGEST_LENGTH = 8;
    std::string spaces_name, spaces_message;

    for (int i = 0; i < longest_name_length; i++)
        spaces_name += ' ';

    for (int i = 0; i < MESSAGE_LONGEST_LENGTH; i++)
        spaces_message += ' ';

    if (out == NULL || printInBoth) {
        consolePainter->ChangeColor(name_colors[name_color]);
        //consolePainter->ChangeColor(TEXT_BLUE|TEXT_INTENSITY);
        fprintf(stderr, "[%s]", timestamp);

        consolePainter->ResetColor();
        fprintf(stderr, " ");
        consolePainter->ChangeColor(name_colors[name_color]);

        fprintf(stderr, "[%s]", name.c_str());
        consolePainter->ResetColor();
        fprintf(stderr, "%s ", spaces_name.c_str() + name.length());
        consolePainter->ChangeColor(col);
        fprintf(stderr, "[%s]%s", message.c_str(), spaces_message.c_str() + message.length());
        consolePainter->ResetColor();
        fprintf(stderr, " ");
        consolePainter->ChangeColor(col);
        fprintf(stderr, log.c_str());
        fprintf(stderr, "\n");
        consolePainter->ResetColor();
    }

    if (out != NULL || printInBoth) {
        fprintf(out, "[%s] ", timestamp);
        fprintf(out, " [%s]%s ", name.c_str(), spaces_name.c_str() + name.length());
        fprintf(out, "[%s]%s ", message.c_str(), spaces_message.c_str() + message.length());
        fprintf(out, log.c_str());
        fprintf(out, "\n");
        fflush(out);
    }
}

void SpecializedLogger::Get_current_time(char *ret) {
    timeval curTime;
    gettimeofday(&curTime, NULL);
    int milliseconds = curTime.tv_usec / 1000;

    time_t raw;
    time(&raw);
    tm *now = localtime(&raw);
    strftime(ret, 100, "%Y-%m-%d %H:%M:%S", now);
    sprintf(ret, "%s.%.3d", ret, milliseconds);
}
}
