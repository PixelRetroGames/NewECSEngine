/*#pragma once

#include "../src/engine/window.h"
#include "../src/engine/resource_manager.h"
#include "../src/ecs/engine.h"
#include "../src/dynamic_loader/wrapper_generators.h"
#include "../src/dynamic_loader/dynamic_loader.h"
#include "../space_shooter_game/components.h"

namespace BIECS {

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

class EngineWrapper {
public:
    static ECS::Engine<ECS::DefaultEngineCore> *globalEngine;
    static inline auto textureSharedManager = Engine::MakeSharedResourceManager(LoadTexture, DeleteTexture);
    static inline auto ttfUniqueManager = Engine::MakeUniqueResourceManager(CreateTTF, DeleteTexture);

    static inline auto fontSharedManager = Engine::MakeSharedResourceManager(LoadFont, DeleteFont);
    static inline ResourceManagerT resourceManager = CreateResourceManager(Engine::UniqueResManagerGroup(ttfUniqueManager),
                                          Engine::SharedResManagerGroup(textureSharedManager, fontSharedManager));
};

EVAL(CREATE_CONTEXT(GameContext, (DynamicLoader::DynamicScriptManager<GameContext>, dynamicLoader),(ECS::Engine<ECS::DefaultEngineCore>, engine), (ResourceManagerT, resourceManager), (Engine::Window, window), (float, dt)))
}
*/
