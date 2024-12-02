#pragma once

#include "../src/ecs/engine.h"
#include "../src/engine/window.h"
#include "components.h"
#include "shared.h"
#include <string>

namespace SpaceShooter {
class HealthRenderer : public ECS::SystemInterface {
  private:
    GameContext *ctx;
    Engine::SharedResource<Engine::Font> font;

  public:
    HealthRenderer(GameContext *ctx,
                   Engine::SharedResource<Engine::Font> font):
        ctx(ctx), font(font) {}

    virtual void Update() override {
        auto textureManager = ctx->resourceManager->GetSharedManager<Engine::Texture, Engine::TextureLoadArgs>();
        auto ttfManager = ctx->resourceManager->GetUniqueManager<Engine::Texture, Engine::TTFTextureLoadArgs>();
        static auto texture = textureManager->Load("assets/red.png");

        for (const auto &[transform2D, hp] : ctx->engine->GetGroupView<Transform2D, Health>()) {
            auto ttf = ttfManager->Load(font.get(), std::to_string(hp), Engine::Color(255, 0, 0));

            Engine::Rectangle pos(transform2D.x, transform2D.y);
            pos.y -= ttf.get()->h;
            pos.w = ttf.get()->w;
            pos.h = ttf.get()->h;

            ctx->window->GetRenderer()->DrawTexture(Engine::Rectangle(), texture.get(), pos);
            ctx->window->GetRenderer()->DrawTexture(
                pos.x, pos.y,
                ttf.get());
        }
    }
};
}
