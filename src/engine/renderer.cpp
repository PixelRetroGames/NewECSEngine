#include "renderer.h"

namespace Engine {

void Renderer::Open(SDL_Window *window, int _flags) {
    flags = _flags;
    renderer = SDL_CreateRenderer(window, -1, _flags);

    if (renderer == nullptr) {
        printf("Renderer Could not create renderer: %s", SDL_GetError());
        LOG_CRITICAL("Renderer", "Could not create renderer: %s", SDL_GetError());
        exit(-1);
    }

    /*rendererMutex = SDL_CreaterendererMutex();

    if (rendererMutex == nullptr) {
        LOG_CRITICAL("Renderer", "Could not create rendererMutex: %s", SDL_GetError());
        exit(-1);
    }*/

    NULL_texture = new Texture(0, 0, nullptr);
    SDL_GetWindowSize(window, &w, &h);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    LOG_INFO("Renderer", "Renderer opened");
}

void Renderer::Close() {
    SDL_DestroyRenderer(renderer);
    w = h = 0;
    LOG_INFO("Renderer", "Renderer closed");
}

void Renderer::ReOpen(SDL_Window *window, int _flags) {
    Close();
    Open(window, _flags);
}

Texture *Renderer::LoadTexture(std::string filename) {
    Impl::Surface *image_surface = surfaceFactory->Load(filename);

    if (image_surface == Impl::NULLSurfacePtr)
        return NULL_texture;

    SDL_Texture *image_texture = nullptr;

    {
        std::unique_lock lock(rendererMutex);
        image_texture = SDL_CreateTextureFromSurface(renderer, image_surface->GetImage());

        if (image_texture == nullptr) {
            LOG_ERROR("Renderer", "Could not load texture: %s", SDL_GetError());
            return NULL_texture;
        }
    }

    Texture *texture;
    texture = new Texture(image_surface->GetW(), image_surface->GetH(), image_texture);

    image_surface->Clear();

    LOG_INFO("Renderer", "Loaded texture: %s", filename.c_str());

    return texture;
}

Texture *Renderer::CreateTransparentTexture(int w, int h) {
    Texture *_texture = nullptr;

    {
        std::unique_lock lock(rendererMutex);
        _texture = new Texture(w, h, SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h));
        SDL_SetRenderTarget(renderer, _texture->GetImage());
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(_texture->GetImage(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, nullptr);
    }

    if (_texture->GetImage() == nullptr) {
        LOG_ERROR("Renderer", "Could not create transparent texture: %s", SDL_GetError());
        return NULL_texture;
    }

    LOG_INFO("Renderer", "Created transparent texture: w=%d,h=%d", w, h);
    return _texture;
}

Texture *Renderer::CreateTTFTexture(Font *font, std::string text, Color color) {
    if (font == nullptr || font->font == nullptr) {
        LOG_ERROR("Renderer", "Could not create TTF texture: font is null");
        return NULL_texture;
    }

    SDL_Surface *image_surface = TTF_RenderText_Blended(font->font, text.c_str(), color.GetSDLColor());
    SDL_Texture *image_texture = nullptr;

    if (image_surface == nullptr)
        return nullptr;

    {
        std::unique_lock lock(rendererMutex);
        image_texture = SDL_CreateTextureFromSurface(renderer, image_surface);

        if (image_texture == nullptr) {
            LOG_ERROR("Renderer", "Could not create TTF texture: %s", SDL_GetError());
            return NULL_texture;
        }
    }

    Texture *texture;
    texture = new Texture(image_surface->w, image_surface->h, image_texture);

    SDL_FreeSurface(image_surface);
    //LOG_INFO("Renderer", "Created TTF texture: text=[%s]", text.c_str());
    return texture;
}

void Renderer::SetTextureBlendMode(Texture *texture, SDL_BlendMode blend_mode) {
    if (texture == nullptr || texture->GetImage() == nullptr)
        return;

    std::lock_guard lock(rendererMutex);
    SDL_SetTextureBlendMode(texture->GetImage(), blend_mode);
}

void Renderer::SetTextureAlpha(Texture *texture, Uint8 alpha) {
    if (texture == nullptr || texture->GetImage() == nullptr || texture->GetAlpha() == alpha)
        return;

    SDL_BlendMode blendmode;
    SDL_GetTextureBlendMode(texture->GetImage(), &blendmode);

    if (blendmode != SDL_BLENDMODE_BLEND)
        SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    SDL_GetTextureBlendMode(texture->GetImage(), &blendmode);

    {
        std::lock_guard lock(rendererMutex);
        SDL_SetTextureAlphaMod(texture->GetImage(), alpha);
    }

    texture->SetAlpha(alpha);
}

void Renderer::DrawTexture(Rectangle src_rect, Texture *src, Rectangle dst_rect) {
    if (src == nullptr || src->GetImage() == nullptr) {
        LOG_ERROR("Renderer", "Could not draw nullptr texture");
        return;
    }

    if (src_rect.w < 1 && src_rect.h < 1) {
        src_rect.w = src->GetW();
        src_rect.h = src->GetH();
    }

    if (dst_rect.w < 1 || dst_rect.h < 1) {
        dst_rect.w = src_rect.w;
        dst_rect.h = src_rect.h;
    }

    {
        std::lock_guard lock(rendererMutex);
        SDL_Rect src_offset = (SDL_Rect) src_rect;
        SDL_Rect dst_offset = (SDL_Rect) dst_rect;
        SDL_RenderCopy(renderer, src->GetImage(), &src_offset, &dst_offset);
    }
}

void Renderer::DrawTexture(int x, int y, Texture *source) {
    if (source == nullptr || source->GetImage() == nullptr) {
        LOG_ERROR("Renderer", "Could not draw nullptr texture");
        return;
    }

    Rectangle offset(x, y, source->GetW(), source->GetH());

    if (x == RENDERERPOS_CENTERED)
        offset.x = (w - source->GetW()) / 2;

    if (y == RENDERERPOS_CENTERED)
        offset.y = (h - source->GetH()) / 2;

    {
        std::lock_guard lock(rendererMutex);
        SDL_Rect _offset = (SDL_Rect) offset;
        SDL_RenderCopy(renderer, source->GetImage(), nullptr, &(_offset));
    }
}

void Renderer::DrawTexture(int x, int y, Texture *source, Texture *destination) {
    if (source == nullptr || source->GetImage() == nullptr) {
        LOG_ERROR("Renderer", "Could not draw nullptr texture");
        return;
    }

    std::lock_guard lock(rendererMutex);

    if (destination == nullptr || destination->GetImage() == nullptr) {
        LOG_ERROR("Renderer", "Could not draw to nullptr destination texture");
        return;
    }

    Rectangle offset(x, y, source->GetW(), source->GetH());

    if (x == RENDERERPOS_CENTERED)
        offset.x = (w - source->GetW()) / 2;

    if (y == RENDERERPOS_CENTERED)
        offset.y = (h - source->GetH()) / 2;

    SDL_SetRenderTarget(renderer, destination->GetImage());
    SDL_Rect _offset = (SDL_Rect) offset;
    SDL_RenderCopy(renderer, source->GetImage(), nullptr, &_offset);
    SDL_SetRenderTarget(renderer, nullptr);
}

void Renderer::DrawRect(const Rectangle &rect) {
    std::lock_guard lock(rendererMutex);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
    SDL_Rect sdlRect = rect;
    SDL_RenderDrawRect(renderer, &sdlRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
}

void Renderer::DrawLine(int x, int y, int x2, int y2, Color color) {
    std::lock_guard lock(rendererMutex);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, x, y, x2, y2);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
}

void Renderer::Flush() {
    std::lock_guard lock(rendererMutex);
    SDL_RenderClear(renderer);
}

void Renderer::Present() {
    std::lock_guard lock(rendererMutex);
    SDL_RenderPresent(renderer);
}
/*
Renderer *RENDERER = nullptr;

void SetGlobalRenderer(Renderer *renderer) {
    RENDERER = renderer;
}

Texture *LoadTexture(std::string filename) {
    if (RENDERER == nullptr)
        return nullptr;

    return RENDERER->LoadTexture(filename);
}*/
}
