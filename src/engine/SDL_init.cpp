#include "SDL_init.h"

namespace Engine {
int status = 0;

void Init() {
    LOG::Init();
    LOG::AddLogger("SDL");
    LOG::AddLogger("Window");
    LOG::AddLogger("Surface");
    LOG::AddLogger("Renderer");
    LOG::AddLogger("ECS");
    LOG::AddLogger("EV BUS");
    LOG::AddLogger("RES MANAGER");
    LOG::AddLogger("Application");
    LOG::AddLogger("Debug");
    InitSDL();
    InitTTF();
    InitMix();
}

void InitSDL() {
    if (status & SDL_INITIALIZED)
        return;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        LOG_CRITICAL("SDL", "SDL_Init failed: %s", SDL_GetError());
        exit(-1);
    }

    status |= SDL_INITIALIZED;

    IMG_Init(IMG_INIT_PNG);
    LOG_INFO("SDL", "SDL opened");
}

void InitTTF() {
    if (status & TTF_INITIALIZED)
        return;

    if (TTF_Init() == -1) {
        LOG_ERROR("SDL", "TTF_Init failed: %s", SDL_GetError());
    }

    status |= TTF_INITIALIZED;
    LOG_INFO("SDL", "SDL_TTF opened");
}

void InitMix() {
    if (status & MIX_INITIALIZED)
        return;

    if ((Mix_Init(MIX_INIT_MP3) & (MIX_INIT_MP3)) != MIX_INIT_MP3) {
        LOG_ERROR("SDL", "Mix_Init failed: %s", SDL_GetError());
    }

    status |= MIX_INITIALIZED;

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 4, 4096) == -1) {
        LOG_ERROR("SDL", "Mix_OpenAudio failed: %s", SDL_GetError());
    }

    LOG_INFO("SDL", "SDL_Mix opened");
}

void Close() {
    CloseTTF();
    CloseMix();
    CloseSDL();
    LOG::Close();
}

void CloseSDL() {
    if (!(status & SDL_INITIALIZED))
        return;

    SDL_Quit();
    status &= ~SDL_INITIALIZED;
    LOG_INFO("SDL", "SDL closed");
}

void CloseTTF() {
    if (!(status & TTF_INITIALIZED))
        return;

    TTF_Quit();
    status &= ~TTF_INITIALIZED;
    LOG_INFO("SDL", "SDL_TTF closed");
}

void CloseMix() {
    if (!(status & MIX_INITIALIZED))
        return;

    Mix_Quit();
    status &= ~MIX_INITIALIZED;
    LOG_INFO("SDL", "SDL_Mix closed");
}

void PrintStatus(FILE *out) {
    fprintf(out, "SDL_INIT=%d, TTF_INIT=%d, MIX_INIT=%d\n", bool(status & SDL_INITIALIZED),
            bool(status & TTF_INITIALIZED),
            bool(status & MIX_INITIALIZED));
}

}

