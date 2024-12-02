#include "bullet_collision_system.h"

namespace SpaceShooter {
void BulletCollisionSystem::Update() {
    /*std::vector<ECS::Entity> entitiesToRemove;
    for (const auto &[entity, collision, hp, destroyOnCollision] : engine->GetGroupView<ECS::Entity, Colliding>(ECS::Optional<Health, DestroyOnCollision>())) {
        engine->DeleteComponents<Colliding>(entity);
        if (destroyOnCollision) {
            entitiesToRemove.push_back(entity);
            continue;
        }

        if (hp) {
            *hp = (*hp) - 1;
            if (*hp == 0) {
                entitiesToRemove.push_back(entity);
            }
        }
    }

    for (auto entity : entitiesToRemove) {
        engine->DeleteEntity(entity);
    }*/
}
};
