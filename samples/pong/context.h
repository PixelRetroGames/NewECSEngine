#pragma once

#include "dynamic_loader/dynamic_script_interface.h"
#include "game_engine/engine.h"
#include "engine/resource_manager.h"

typedef Engine::SharedResource<Engine::Texture> SharedTexturePtr;
typedef Engine::UniqueResource<Engine::Texture> UniqueTexturePtr;

extern Engine::Window *window;

Engine::Texture *LoadTexture(Engine::TextureLoadArgs args);

void DeleteTexture(Engine::Texture *texture);

typedef Engine::SharedResource<Engine::Font> FontPtr;

Engine::Font *LoadFont(Engine::FontLoadArgs args);

void DeleteFont(Engine::Font *font);

Engine::Texture *CreateTTF(Engine::TTFTextureLoadArgs args);

#define ResourceManagerT decltype(\
    CreateResourceManager(Engine::UniqueResManagerGroup(Engine::MakeUniqueResourceManager(CreateTTF, DeleteTexture)),\
                          Engine::SharedResManagerGroup(Engine::MakeSharedResourceManager(LoadTexture, DeleteTexture), Engine::MakeSharedResourceManager(LoadFont, DeleteFont))))

EVAL(CREATE_CONTEXT(GameContext, (ECS::Engine<ECS::DefaultEngineCore>, engine), (ResourceManagerT, resourceManager), (Engine::Window, window), (float, dt)))

