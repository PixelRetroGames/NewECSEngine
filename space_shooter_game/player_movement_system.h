#pragma once

#include "shared.h"

namespace SpaceShooter {

class PlayerMovementSystem : public ECS::SystemInterface {
  private:
    GameContext *ctx;

  public:
    PlayerMovementSystem(GameContext *ctx):
        ctx(ctx) {}

    virtual void Update() override;
};

}
