#pragma once

#include "../src/ecs/engine.h"
#include "../src/engine/window.h"
#include "components.h"

namespace SpaceShooter {

typedef Engine::SharedResource<Engine::Texture> SharedTexturePtr;
typedef Engine::UniqueResource<Engine::Texture> UniqueTexturePtr;

class TextureRendererSystem : public ECS::SystemInterface {
private:
    Engine::Window* &window;
    ECS::Engine<ECS::InefficientEngineCore>* &engine;

public:
    TextureRendererSystem(Engine::Window* &window,
        ECS::Engine<ECS::InefficientEngineCore>* &engine):
        window(window), engine(engine) {}

    virtual void Update() override;
};

}

