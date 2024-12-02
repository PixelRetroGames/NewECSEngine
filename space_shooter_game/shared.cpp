#include "shared.h"

namespace SpaceShooter {

Engine::Window *window;

Engine::Texture *LoadTexture(Engine::TextureLoadArgs args) {
    return window->GetRenderer()->LoadTexture(args.path);
}

void DeleteTexture(Engine::Texture *texture) {
    texture->Clear();
    delete texture;
}

Engine::Font *LoadFont(Engine::FontLoadArgs args) {
    return Engine::LoadFont(args.path, args.size);
}

void DeleteFont(Engine::Font *font) {
    ClearFont(font);
}

Engine::Texture *CreateTTF(Engine::TTFTextureLoadArgs args) {
    return window->GetRenderer()->CreateTTFTexture(args.font, args.text, args.color);
}

}
