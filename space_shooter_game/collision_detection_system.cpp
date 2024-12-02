#include "collision_detection_system.h"

namespace SpaceShooter {

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
    entitiesColliders.clear();

    for (auto const &[entity, oldTransform, colliderOriginal, optNewTranform, optScaling] : engine->GetGroupView<ECS::Entity, Transform2D, Collider>(ECS::Optional<NewTransform2D, Scaling>())) {
        Scaling scaling(1, 1);

        if (optScaling) {
            scaling = *optScaling;
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
                auto collision = engine->CreateEntity();
                engine->AddComponent(collision, Collision(entitiesColliders[i].first, entitiesColliders[j].first));
            }
        }
    }
}
};
