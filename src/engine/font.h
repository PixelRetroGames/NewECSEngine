#pragma once

#include <string>

#include "SDL_TTF.h"
#include "utils.h"
#include "../logging/logging.h"

namespace Engine {
class Font {
  public:
    TTF_Font *font = NULL;

    void Load(std::string filename, int ptrSize);
    void Clear();
};

Font *LoadFont(std::string filename, int size);
void ClearFont(Font *font);

class FontLoadArgs {
  public:
    std::string path;
    int size;

    FontLoadArgs(std::string path, int size = 20) :
        path(path), size(size) {}

    bool operator==(const FontLoadArgs& other) const {
        return (path == other.path) &&
               (size == other.size);
    }
};
}

namespace std {
template <>
struct hash<Engine::FontLoadArgs> {
    size_t operator()(const Engine::FontLoadArgs &p) const {
        size_t path = hash<string>()(p.path);
        size_t size = hash<int>()(p.size);
        return path ^ (size << 1);  // Combine the hash values
    }
};
}
