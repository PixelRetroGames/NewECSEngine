#pragma once

#include "../src/ecs/engine.h"
#include "../src/engine/window.h"
#include "components.h"

namespace SpaceShooter {

class VelocityMovementSystem : public ECS::SystemInterface {
private:
    Engine::Window* &window;
    float &dt;
    ECS::Engine<ECS::InefficientEngineCore>* &engine;
    Collider &windowPos;

public:
    VelocityMovementSystem(Engine::Window* &window,
        float &dt,
        ECS::Engine<ECS::InefficientEngineCore>* &engine,
        Collider &windowPos):
        window(window), dt(dt), engine(engine), windowPos(windowPos) {}

    virtual void Update() override;
};

}
