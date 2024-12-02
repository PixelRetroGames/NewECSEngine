#include "utils.h"

namespace Engine {
void Rectangle::Print(FILE *out) {
    if (out == NULL)
        out = stderr;

    fprintf(stderr, "Rect {%f, %f, %f, %f}\n", x, y, w, h);
}

SDL_Color Color::GetSDLColor() {
    SDL_Color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}

void Timer::Start() {
    start = timer::now();
}

float Timer::GetTime() {
    timer::time_point end;
    end = timer::now();
    //float aux = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    return 0.001f * (float)(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}

}
