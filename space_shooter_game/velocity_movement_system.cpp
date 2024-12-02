#include "velocity_movement_system.h"

namespace SpaceShooter {
void VelocityMovementSystem::Update() {
    //for (const auto &[transform2D, velocity2D, colliderOriginal, scaling] : engine->GetGroupView<Transform2D, Velocity2D, Collider, Scaling>()) {
    for (const auto &[transform2D, velocity2D, scaling, colliderOriginal] : engine->GetGroupView<Transform2D, Velocity2D, Scaling>(ECS::Optional<Collider>())) {
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
};
