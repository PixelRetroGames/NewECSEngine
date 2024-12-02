#include "surface.h"

namespace Engine {

namespace Impl {

Surface::Surface() {
}

Surface::Surface(SDL_Surface *_surf) {
    if (_surf == NULL) {
        return;
    }

    image = _surf;
    w = _surf->w;
    h = _surf->h;
}

Surface::Surface(int _w, int _h, SDL_Surface *_image) {
    w = _w;
    h = _h;
    image = _image;
}

int Surface::GetW() {
    return w;
}

int Surface::GetH() {
    return h;
}

SDL_Surface *Surface::GetImage() {
    return image;
}

void Surface::Clear() {
    LOG_INFO("Surface", "Surface cleared");
    SDL_FreeSurface(image);
    image = NULL;
    w = h = 0;
}

}
}
