#pragma once

#include <thread>
#include "texture.h"
#include "font.h"
#include "surface_factory.h"

namespace Engine {

enum Renderer_position {
    RENDERERPOS_CENTERED = 1 << 10
};

class Renderer {
  private:
    const std::unique_ptr<Impl::SurfaceFactory> surfaceFactory;
    SDL_Renderer *renderer = NULL;
    Texture *NULL_texture = NULL;
    std::mutex rendererMutex;
    int flags = 0;
    int w = 0, h = 0;

  public:
    void Open(SDL_Window *window, int _flags);
    void Close();
    void ReOpen(SDL_Window *Window, int _flags);

    Texture *LoadTexture(std::string filename);
    Texture *CreateTransparentTexture(int w, int h);
    Texture *CreateTTFTexture(Font *font, std::string text, Color color);

    void SetTextureBlendMode(Texture *texture, SDL_BlendMode blend_mode);
    void SetTextureAlpha(Texture *texture, Uint8 alpha);

    void DrawTexture(Rectangle src_rect, Texture *src, Rectangle dst_rect);
    void DrawTexture(int x, int y, Texture *source);
    void DrawTexture(int x, int y, Texture *source, Texture *destination);

    void DrawRect(const Rectangle &rect);
    void DrawLine(int x, int y, int x2, int y2, Color color);

    void Flush();
    void Present();
};

/*extern Renderer *RENDERER;
void SetGlobalRenderer(Renderer *renderer);
Texture *LoadTexture(std::string filename);*/
}
