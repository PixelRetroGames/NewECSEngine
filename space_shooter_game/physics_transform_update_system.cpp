#include "physics_transform_update_system.h"

namespace SpaceShooter {
void PhysicsTransformUpdateSystem::Update() {
    //std::vector<ECS::Entity> entities;

    for (const auto &[entity, transform2D, newTransform] : ctx->engine->GetGroupView<ECS::Entity, Transform2D, NewTransform2D>()) {
        transform2D = newTransform;
        ctx->engine->DeleteComponents<NewTransform2D>(entity);
        //entities.push_back(entity);
    }

    /*for (auto ent : entities) {
        ctx->engine->DeleteComponents<NewTransform2D>(ent);
    }*/
}
}

