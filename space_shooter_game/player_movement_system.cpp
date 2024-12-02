#include "player_movement_system.h"

namespace SpaceShooter {
void PlayerMovementSystem::Update() {
    for (const auto &[transform2D, renderRect, scaling] : ctx->engine->GetGroupView<Transform2D, RenderRect, Scaling>(ECS::Unused<PlayerMovement>())) {
        int dirX = 0, dirY = 0;

        if (ctx->window->GetKeystate(Engine::Scancode::SCANCODE_LEFT)) {
            dirX = -1;
        } else if (ctx->window->GetKeystate(Engine::Scancode::SCANCODE_RIGHT)) {
            dirX = 1;
        }

        if (ctx->window->GetKeystate(Engine::Scancode::SCANCODE_UP)) {
            dirY = -1;
        } else if (ctx->window->GetKeystate(Engine::Scancode::SCANCODE_DOWN)) {
            dirY = 1;
        }

        float dt = *(ctx->dt);

        transform2D.x += dirX * 1500 * dt;
        transform2D.x = std::min(transform2D.x, window->GetPosition().w - renderRect.w * scaling.x);
        transform2D.x = std::max(transform2D.x, 0.0f);

        transform2D.y += dirY * 1500 * dt;
        transform2D.y = std::min(transform2D.y, window->GetPosition().h - renderRect.h * scaling.y);
        transform2D.y = std::max(transform2D.y, 0.0f);
    }
}
};
