#pragma once

#include "../src/ecs/engine.h"
#include "../src/engine/window.h"
#include "components.h"

namespace SpaceShooter {

class CollisionDetectionSystem : public ECS::SystemInterface {
  private:
    Engine::Window* &window;
    float &dt;
    ECS::Engine<ECS::DefaultEngineCore>* &engine;
    Collider &windowPos;

    std::vector<std::pair<ECS::Entity, Collider>> entitiesColliders;

  public:
    CollisionDetectionSystem(Engine::Window* &window,
                             float &dt,
                             ECS::Engine<ECS::DefaultEngineCore>* &engine,
                             Collider &windowPos):
        window(window), dt(dt), engine(engine), windowPos(windowPos) {}

    virtual void Update() override;
};

}
