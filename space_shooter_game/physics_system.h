#pragma once

#include "shared.h"

namespace SpaceShooter {

class PhysicsSystem : public ECS::SystemInterface {
  private:
    GameContext *ctx;
    Collider &windowPos;

  public:
    PhysicsSystem(GameContext *ctx, Collider &windowPos):
        ctx(ctx), windowPos(windowPos) {}

    virtual void Update() override;
};

}

