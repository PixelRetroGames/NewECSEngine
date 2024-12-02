#pragma once

#include "shared.h"

namespace SpaceShooter {

class PhysicsTransformUpdateSystem : public ECS::SystemInterface {
  private:
    GameContext *ctx;

  public:
    PhysicsTransformUpdateSystem(GameContext *ctx):
        ctx(ctx) {}

    virtual void Update() override;
};

}


