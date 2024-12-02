#pragma once

#include "shared.h"

namespace SpaceShooter {

class ColliderDebugSystem : public ECS::SystemInterface {
  private:
    GameContext *ctx;


  public:
    ColliderDebugSystem(GameContext *ctx):
        ctx(ctx) {}

    virtual void Update() override {
        auto fontManager = ctx->resourceManager->GetSharedManager<Engine::Font, Engine::FontLoadArgs>();
        auto ttfManager = ctx->resourceManager->GetUniqueManager<Engine::Texture, Engine::TTFTextureLoadArgs>();

        for (const auto &[entity, oldCollider, transform2D, optScaling, optVelocity] : ctx->engine->GetGroupView<ECS::Entity, Collider, Transform2D>(ECS::Optional<Scaling, Velocity2D>())) {
            Scaling scaling(1, 1);

            if (optScaling) {
                scaling = *optScaling;
            }

            Collider collider = oldCollider;
            collider.x += transform2D.x;
            collider.y += transform2D.y;

            collider.w *= scaling.x;
            collider.h *= scaling.y;

            /*window->GetRenderer()->DrawTexture(
                   Engine::Rectangle(),
                   texture.get(),
                   collider);*/

            if (optVelocity) {
                Velocity2D velocity = *optVelocity;
                static auto font = fontManager->Load("consola.ttf", 20);
                auto ttf = ttfManager->Load(font.get(), velocity.to_string(), Engine::Color(255, 0, 0));
                ctx->window->GetRenderer()->DrawTexture(
                    collider.x + collider.w, collider.y,
                    ttf.get());

                glm::vec2 pos1(collider.x + collider.w / 2, collider.y + collider.h / 2);
                glm::vec2 pos2 = pos1 + glm::normalize(velocity) * 60.0f;
                ctx->window->GetRenderer()->DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, Engine::Color(255, 0, 0));
            }

            ctx->window->GetRenderer()->DrawRect(collider);

            {
                static auto font = fontManager->Load("consola.ttf", 30);
                auto ttf = ttfManager->Load(font.get(), std::to_string(entity), Engine::Color(255, 0, 255));
                ctx->window->GetRenderer()->DrawTexture(
                    collider.x + collider.w, collider.y + 20,
                    ttf.get());
            }
        }
    }
};

}


