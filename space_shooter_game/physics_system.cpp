#include "physics_system.h"

namespace SpaceShooter {
void PhysicsSystem::Update() {
    for (const auto &[entity, transform2D, velocity2D] : ctx->engine->GetGroupView<ECS::Entity, Transform2D, Velocity2D>()) {
        float dt = *(ctx->dt);

        NewTransform2D newTranform = transform2D;

        newTranform += dt * velocity2D;

        /*newTranform.x += dt * velocity2D.x;
        newTranform.y += dt * velocity2D.y;*/

        ctx->engine->AddComponent(entity, newTranform);

        //transform2D = newTranform;

        //transform2D.y += dt * velocity2D.y;
    }
}
}

