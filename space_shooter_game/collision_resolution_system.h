#pragma once

#include "shared.h"

namespace SpaceShooter {

class CollisionResolutionSystem : public ECS::SystemInterface {
  private:
    GameContext *ctx;

  public:
    CollisionResolutionSystem(GameContext *ctx):
        ctx(ctx) {}

    virtual void Update() override;
};

}

