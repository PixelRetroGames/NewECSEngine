#pragma once

#include <cstdio>
#include <chrono>
#include <tuple>

#include "SDL.h"

namespace Engine {

class Rectangle {
  public:
    float x = 0, y = 0, w = 0, h = 0;

    Rectangle() {}
    Rectangle(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
    Rectangle(float x, float y) : x(x), y(y) {}
    Rectangle(SDL_Rect *rect) : Rectangle(rect->x, rect->y, rect->w, rect->h) {}
    Rectangle(const Rectangle &other) : Rectangle(other.x, other.y, other.w, other.h) {}

    operator SDL_Rect() const {
        return SDL_Rect{(int) x, (int) y, (int) w, (int) h};
    }

    void Print(FILE *out = NULL);

    Rectangle &operator=(const SDL_Rect &other) {
        x = other.x;
        y = other.y;
        w = other.w;
        h = other.h;
        return *this;
    }
};

class Color {
  public:
    int r = 0, g = 0, b = 0, a = 0;

    Color(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) {}
    SDL_Color GetSDLColor();

    bool operator==(const Color& other) const {
        return std::make_tuple(r, g, b, a) == std::make_tuple(other.r, other.g, other.b, other.a);
    }
};

class Timer {
  private:
    typedef std::chrono::system_clock timer;
    timer::time_point start;
    timer::duration duration;

  public:
    Timer() {
        Start();
    }

    void Start();
    float GetTime();
};

/*template <typename DataT, typename KeyT, typename CmpFnT>
std::vector<DataT>::iterator binary_search(const std::vector<DataT> &v, const KeyT &key, CmpFnT &cmpFn) {

}*/

}
