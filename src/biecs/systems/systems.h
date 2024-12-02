#pragma once

// Built-in templated systems

#include "physics/physics.h"

namespace BIECS {

StatelessSystem(VelocityMovementSystem, GameContext);

void VelocityMovementSystem::Update() {
    auto aux = window->GetPosition();
    auto windowPos = Collider(aux.x, aux.y, aux.w, aux.h);
    auto dt = *ctx->dt;

    Collider collider;
    Transform2D newTransform2D;

    for (const auto &[transform2D, velocity2D, scaling, optCollider] :
            ctx->engine->GetGroupView<Transform2D, Velocity2D, Scaling>(ECS::Optional<Collider>(), ECS::Unused<VelocityMoved>())) {
        if (optCollider != nullptr) {
            collider = *optCollider;
        } else {
            collider = Collider();
        }

        newTransform2D = transform2D + dt * velocity2D;

        collider.x += newTransform2D.x;
        collider.y += newTransform2D.y;

        collider.w *= scaling.x;
        collider.h *= scaling.y;

        bool collidedX = false;
        bool collidedY = false;

        if (collider.x + collider.w > windowPos.w) {
            collidedX = true;
        }

        if (collider.x < 0) {
            collidedX = true;
        }

        if (collider.y + collider.h > windowPos.h) {
            collidedY = true;
        }

        if (collider.y < 0) {
            collidedY = true;
        }

        if (!collidedX && !collidedY) {
            transform2D = newTransform2D;
        }

        if (collidedX) {
            transform2D.y = newTransform2D.y;
        }

        if (collidedY) {
            transform2D.x = newTransform2D.x;
        }
    }
}

StatelessSystem(TextureRendererSystem, GameContext);

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

using CollisionFilter = std::function<void(ECS::Entity, const Collision&, GameContext *ctx)>;

class CollisionDetectionSystem : public ECS::SystemInterface {
  private:
    GameContext *ctx;
    std::vector<CollisionFilter> filters;

  public:
    CollisionDetectionSystem(GameContext *ctx):
        ctx(ctx) {}

    CollisionDetectionSystem(GameContext *ctx, std::vector<CollisionFilter> &filters):
        ctx(ctx), filters(filters) {}

    void AddFilter(CollisionFilter filter) {
        filters.push_back(filter);
    }

    virtual void Update() override;
};

static bool CollidesX(Collider &a, Collider &b) {
    return (a.x >= b.x && a.x <= b.x + b.w) ||
           (b.x >= a.x && b.x <= a.x + a.w);
}

static bool CollidesY(Collider &a, Collider &b) {
    return (a.y >= b.y && a.y <= b.y + b.h) ||
           (b.y >= a.y && b.y <= a.y + a.h);
}

static bool Collides(Collider &a, Collider &b) {
    return CollidesX(a, b) && CollidesY(b, a);
}

void CollisionDetectionSystem::Update() {
    std::vector<std::pair<ECS::Entity, Collider>> entitiesColliders;
    Scaling scaling(1, 1);

    for (auto const &[entity, oldTransform, colliderOriginal, optNewTranform, optScaling] : ctx->engine->GetGroupView<ECS::Entity, Transform2D, Collider>(ECS::Optional<NewTransform2D, Scaling>())) {
        if (optScaling) {
            scaling = *optScaling;
        } else {
            scaling = Scaling(1, 1);
        }

        Transform2D transform2D = oldTransform;

        if (optNewTranform) {
            transform2D = *optNewTranform;
        }

        Collider collider = colliderOriginal;

        collider.x += transform2D.x;
        collider.y += transform2D.y;

        collider.w *= scaling.x;
        collider.h *= scaling.y;
        entitiesColliders.push_back({entity, collider});
    }

    for (size_t i = 0; i < entitiesColliders.size(); i++) {
        for (size_t j = i + 1; j < entitiesColliders.size(); j++) {
            if (Collides(entitiesColliders[i].second, entitiesColliders[j].second)) {
                auto collision = ctx->engine->CreateEntity();
                auto collisionComponent = Collision(entitiesColliders[i].first, entitiesColliders[j].first);
                ctx->engine->AddComponent(collision, collisionComponent);

                for (auto &filter : filters) {
                    filter(collision, collisionComponent, ctx);
                }
            }
        }
    }
}

StatelessSystem(CollisionClearSystem, GameContext);

void CollisionClearSystem::Update() {
    std::vector<ECS::Entity> entitiesToRemove;

    for (auto [entity, collision] : ctx->engine->GetGroupView<ECS::Entity, Collision>()) {
        entitiesToRemove.push_back(entity);
    }

    ctx->engine->DeleteEntities(entitiesToRemove);
}

}
