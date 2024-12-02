#pragma once

#include "SDL_includes.h"
#include "../logging/logging.h"

namespace Engine {

namespace Impl {

class Surface {
  private:
    int w = 0, h = 0;
    SDL_Surface *image = NULL;

  public:
    Surface();
    Surface(SDL_Surface *_surf);
    Surface(int _w, int _h, SDL_Surface *_image);
    int GetW();
    int GetH();
    SDL_Surface *GetImage();
    void Clear();
};

}
}
