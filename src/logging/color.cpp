#include "color.h"

namespace LOG {

void ConsolePainter::Init() {
    console = GetStdHandle(STD_OUTPUT_HANDLE);
}

void ConsolePainter::ChangeColor(int color) {
    SetConsoleTextAttribute(console, color);
}

void ConsolePainter::ResetColor() {
    SetConsoleTextAttribute(console, TEXT_WHITE | TEXT_INTENSITY);
}

}
