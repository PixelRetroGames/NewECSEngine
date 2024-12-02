#include "texture.h"

namespace Engine {

Texture::Texture(int _w, int _h, SDL_Texture *_image) {
    w = _w;
    h = _h;
    image = _image;
}

void Texture::SetAlpha(int _alpha) {
    alpha = _alpha;
}

int Texture::GetW() {
    return w;
}

int Texture::GetH() {
    return h;
}

int Texture::GetAlpha() {
    return alpha;
}

SDL_Texture *Texture::GetImage() {
    return image;
}

void Texture::Clear() {
    if (image != nullptr) {
        SDL_DestroyTexture(image);
    }

    image = nullptr;;
    w = h = 0;
    alpha = 255;
}

void ClearTexture(Texture *texture) {
    texture->Clear();
    texture = nullptr;
    LOG_INFO("Renderer", "Destroyed texture");
}

}
