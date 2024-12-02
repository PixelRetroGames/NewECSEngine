#pragma once

#include "color.h"
#include "log_query.h"

#include <cstdio>
#include <string>
#include <cstdarg>
#include <ctime>
#include <sys/time.h>
#include <algorithm>
#include <queue>
#include <string>

namespace LOG {
class SpecializedLogger {
  private:
    bool enabled = true;
    FILE *out = NULL;
    std::string name;
    ConsolePainter *consolePainter;
    bool printInBoth;
    static int longest_name_length;
    int name_color;
    static const int NUMBER_OF_NAME_COLORS = 16;
    static const int name_colors[NUMBER_OF_NAME_COLORS];

  public:
    SpecializedLogger(std::string _name, ConsolePainter *_cp, FILE *_out = NULL, bool _fileAndConsole = false, int _name_color = 0):
        out(_out), name(_name), consolePainter(_cp), printInBoth(_fileAndConsole), name_color(_name_color) {
        longest_name_length = std::max(longest_name_length, (int)name.length());
        name_color %= NUMBER_OF_NAME_COLORS;
    }

    void Enable();
    void Disable();
    void Log(LogQuery *query);
    static void Reset_longest_name_length();

  private:
    void Print_query(LogQuery *query);
    void Get_current_time(char *ret);
    int GetNumberOfArguments(const char *format);
};

}
