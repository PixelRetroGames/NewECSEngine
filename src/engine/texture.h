#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"
#include "font.h"
#include "../logging/logging.h"

namespace Engine {
class Texture {
  public:
    int w = 0, h = 0;
    SDL_Texture *image = NULL;
    int alpha = 255;

  public:
    Texture(int _w, int _h, SDL_Texture *_image);
    void SetAlpha(int _alpha);
    int GetW();
    int GetH();
    int GetAlpha();
    SDL_Texture *GetImage();
    void Clear();

    operator Rectangle() {
        return Rectangle(0, 0, w, h);
    }
};

class TextureLoadArgs {
  public:
    std::string path;
    int alpha;

    TextureLoadArgs(std::string path, int alpha = 255) :
        path(path), alpha(alpha) {}

    bool operator==(const TextureLoadArgs& other) const {
        return (path == other.path) &&
               (alpha == other.alpha);
    }
};

class TTFTextureLoadArgs {
  public:
    Font *font;
    std::string text;
    Color color;

    TTFTextureLoadArgs(Font *font, std::string text, Color color):
        font(font), text(text), color(color) {}

    bool operator==(const TTFTextureLoadArgs& other) const {
        return (font == other.font) &&
               (text == other.text) &&
               (color == other.color);
    }
};

void ClearTexture(Texture *texture);
}

namespace std {
template <>
struct hash<Engine::TextureLoadArgs> {
    size_t operator()(const Engine::TextureLoadArgs &p) const {
        size_t path = hash<string>()(p.path);
        size_t alpha = hash<int>()(p.alpha);
        return path ^ (alpha << 1);  // Combine the hash values
    }
};
}

#endif // TEXTURE_H
