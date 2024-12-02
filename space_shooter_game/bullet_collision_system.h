#pragma once

#include "../src/ecs/engine.h"
#include "../src/engine/window.h"
#include "components.h"

namespace SpaceShooter {

class BulletCollisionSystem : public ECS::SystemInterface {
  private:
    Engine::Window* &window;
    float &dt;
    ECS::Engine<ECS::DefaultEngineCore>* &engine;

  public:
    BulletCollisionSystem(Engine::Window* &window,
                          float &dt,
                          ECS::Engine<ECS::DefaultEngineCore>* &engine):
        window(window), dt(dt), engine(engine) {}

    virtual void Update() override;
};

}

