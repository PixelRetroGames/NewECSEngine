#include "systems.h"

namespace SpaceShooter {
void VelocityMovementSystem::Update() {
    auto aux = window->GetPosition();
    auto windowPos = Collider(aux.x, aux.y, aux.w, aux.h);
    auto dt = *ctx->dt;

    for (const auto &[transform2D, velocity2D, scaling, colliderOriginal] : ctx->engine->GetGroupView<Transform2D, Velocity2D, Scaling>(ECS::Optional<Collider>())) {
        //auto  = *it;
        Collider collider;

        if (colliderOriginal != nullptr) {
            collider = *colliderOriginal;
        }

        transform2D.x += dt * velocity2D.x;
        transform2D.y += dt * velocity2D.y;

        collider.x += transform2D.x;
        collider.y += transform2D.y;

        collider.w *= scaling.x;
        collider.h *= scaling.y;

        bool collidedX = false;
        bool collidedY = false;

        if (collider.x + collider.w > windowPos.w) {
            velocity2D.x = -velocity2D.x;
            collider.x = windowPos.w - collider.w - (collider.x - windowPos.w);
            collidedX = true;
        }

        if (collider.x < 0) {
            velocity2D.x = -velocity2D.x;
            collider.x = -collider.x;
            collidedX = true;
        }

        if (collider.y + collider.h > windowPos.h) {
            velocity2D.y = -velocity2D.y;
            collider.y = windowPos.h - collider.h - (collider.y - windowPos.h);
            collidedY = true;
        }

        if (collider.y < 0) {
            velocity2D.y = -velocity2D.y;
            collidedY = true;
            collider.y = -collider.y;
        }

        if (collidedX) {
            transform2D.x = collider.x;
        }

        if (collidedY) {
            transform2D.y = collider.y;
        }
    }
}

void TextureRendererSystem::Update() {
    for (const auto &[transform2D, optRenderRect, optScaling, optSharedTexturePtr, optUniqueTexturePtr] : ctx->engine->GetGroupView<Transform2D>(ECS::Optional<RenderRect, Scaling, SharedTexturePtr, UniqueTexturePtr>())) {
        Engine::Texture *texture = nullptr;

        if (optSharedTexturePtr) {
            texture = optSharedTexturePtr->get();
        } else if (optUniqueTexturePtr) {
            texture = optUniqueTexturePtr->get();
        } else {
            continue;
        }

        RenderRect renderRect = (Engine::Rectangle) * texture;

        if (optRenderRect) {
            renderRect = *optRenderRect;
        }

        Scaling scaling;

        if (optScaling) {
            scaling = *optScaling;
        }

        ctx->window->GetRenderer()->DrawTexture(renderRect,
                                                texture,
                                                Engine::Rectangle(transform2D.x, transform2D.y, scaling.x * (float)renderRect.w, scaling.y * (float)renderRect.h));
    }
}

void BulletShooterSystem::Update() {
    auto manager = ctx->resourceManager->GetSharedManager<Engine::Texture, Engine::TextureLoadArgs>();

    for (const auto &[transform2D, bulletShooter, shooterTexture, scaling] : ctx->engine->GetGroupView<Transform2D, BulletShooter, SharedTexturePtr, Scaling>()) {
        if (!ctx->window->GetKeystate(bulletShooter.key)) {
            continue;
        }

        if (bulletShooter.timer.GetTime() < 0.3) {
            continue;
        }

        auto bullet = ctx->engine->CreateEntity();

        auto texture = manager->Load("assets/red_bullet.png");

        ctx->engine->AddComponent(bullet, texture);
        ctx->engine->AddComponent(bullet, Transform2D(transform2D.x + shooterTexture->w / 2 * scaling.x - texture->w / 2 * 0.1, transform2D.y - texture->h * 0.1));
        ctx->engine->AddComponent(bullet, RenderRect(0, 0, texture->w, texture->h));
        ctx->engine->AddComponent(bullet, Collider(texture->w * 0.25 * 0.1, 0, texture->w * 0.5, texture->h));
        ctx->engine->AddComponent(bullet, Velocity2D(0, -1000));
        ctx->engine->AddComponent(bullet, Scaling(0.1, 0.1));
        //ctx->engine->AddComponent(bullet, DestroyOnCollision());

        bulletShooter.timer.Start();
    }
}
};
