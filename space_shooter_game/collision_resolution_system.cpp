#include "collision_resolution_system.h"

#include <unordered_set>

namespace SpaceShooter {

struct CollidingEntity {
    Transform2D *oldTransform2D;
    Velocity2D *velocity;
    Transform2D *transform2D;
    Collider *collider;
    Scaling *scaling;
    ReflectCollider *reflectCollider;
    float invMass = 1;
    float restitution = 1;
};

static void ResolveCollision(CollidingEntity A, CollidingEntity B) {
    // Calculate relative velocity
    glm::vec2 rv = (*A.velocity) - (*B.velocity);
    // Calculate relative velocity in terms of the normal direction
    auto centerA = (*A.transform2D) + glm::vec2(A.collider->w / 2, A.collider->h / 2);
    auto centerB = (*B.transform2D) + glm::vec2(B.collider->w / 2, B.collider->h / 2);
    glm::vec2 normal = centerA - centerB;
    normal = glm::normalize(normal);

    float velAlongNormal = glm::dot(rv, normal);
    float e = std::min(A.restitution, B.restitution);

    // Do not resolve if velocities are separating
    if (velAlongNormal > 0) {
        return;
    }

    // Calculate restitution
    //float e = 0.5;
    // Calculate impulse scalar
    float j = -(1 + e) * velAlongNormal;

    j /= (A.invMass + B.invMass);
    // Apply impulse
    glm::vec2 impulse = j * normal;
    (*A.velocity) += impulse * A.invMass;
    (*B.velocity) -= impulse * B.invMass;
}

static void ResolveCollision2(CollidingEntity A, CollidingEntity B) {
    auto centerA = (*A.transform2D) + glm::vec2(A.collider->w / 2, A.collider->h / 2);
    auto centerB = (*B.transform2D) + glm::vec2(B.collider->w / 2, B.collider->h / 2);
    glm::vec2 normal = centerA - centerB;

    if (glm::abs(normal.x) < glm::abs(normal.y)) {
        normal.y = 0.0;
    } else {
        normal.x = 0.0;
    }

    normal = glm::normalize(normal);

    (*A.velocity) = ((*A.velocity) - 2 * glm::dot(*A.velocity, normal) * normal);
}

void CollisionResolutionSystem::Update() {
    std::vector<ECS::Entity> entities;
    std::unordered_set<ECS::Entity> newTransformRemove;

    for (const auto &[entity, collision] : ctx->engine->GetGroupView<ECS::Entity, Collision>()) {
        entities.push_back(entity);

        CollidingEntity ent1, ent2;

        ent1.velocity = ctx->engine->GetComponent<Velocity2D>(collision.entities[0]);
        ent2.velocity = ctx->engine->GetComponent<Velocity2D>(collision.entities[1]);

        ent1.transform2D = ctx->engine->GetComponent<NewTransform2D>(collision.entities[0]);

        if (ent1.transform2D == nullptr) {
            ent1.transform2D = ctx->engine->GetComponent<Transform2D>(collision.entities[0]);
        }

        ent1.oldTransform2D = ctx->engine->GetComponent<Transform2D>(collision.entities[0]);
        ent2.oldTransform2D = ctx->engine->GetComponent<Transform2D>(collision.entities[1]);

        ent2.transform2D = ctx->engine->GetComponent<NewTransform2D>(collision.entities[1]);

        if (ent2.transform2D == nullptr) {
            ent2.transform2D = ctx->engine->GetComponent<Transform2D>(collision.entities[1]);
        }

        ent1.collider = ctx->engine->GetComponent<Collider>(collision.entities[0]);
        ent2.collider = ctx->engine->GetComponent<Collider>(collision.entities[1]);

        ent1.reflectCollider = ctx->engine->GetComponent<ReflectCollider>(collision.entities[0]);
        ent2.reflectCollider = ctx->engine->GetComponent<ReflectCollider>(collision.entities[1]);

        ent1.scaling = ctx->engine->GetComponent<Scaling>(collision.entities[0]);
        ent2.scaling = ctx->engine->GetComponent<Scaling>(collision.entities[1]);

        if (ent1.velocity == nullptr && ent2.velocity == nullptr) {
            //LOG_INFO("debug", "Collision ignored");
            continue;
        }

        auto collider1 = *ent1.collider;
        auto collider2 = *ent2.collider;

        if (ent1.scaling) {
            collider1.w *= ent1.scaling->x;
            collider1.h *= ent1.scaling->y;
        }

        if (ent2.scaling) {
            collider2.w *= ent2.scaling->x;
            collider2.h *= ent2.scaling->y;
        }

        collider1.x += ent1.transform2D->x;
        collider1.y += ent1.transform2D->y;

        collider2.x += ent2.transform2D->x;
        collider2.y += ent2.transform2D->y;

        ent1.collider = &collider1;
        ent2.collider = &collider2;

        if (ent1.transform2D->x > ent2.transform2D->x) {
            std::swap(ent1, ent2);
        }

        if (ent1.velocity && ent2.velocity) {
            ent1.restitution = 0.1;
            ent2.restitution = 0.9;
            ResolveCollision(ent1, ent2);

            *(ent1.oldTransform2D) += *(ctx->dt) * (*ent1.velocity);
            *(ent2.oldTransform2D) += *(ctx->dt) * (*ent2.velocity);

            //newTransformRemove.insert(collision.entities[0]);
            //newTransformRemove.insert(collision.entities[1]);
            continue;
        }

        if (ent2.velocity) {
            std::swap(ent1, ent2);
        }

        ent2.restitution = 1;
        ent2.invMass = 0;
        Velocity2D vel(0, 0);
        ent2.velocity = &vel;

        ResolveCollision2(ent1, ent2);

        *(ent1.oldTransform2D) += *(ctx->dt) * 2.0f * (*ent1.velocity);

        //newTransformRemove.insert(collision.entities[0]);
        //newTransformRemove.insert(collision.entities[1]);
    }

    ctx->engine->DeleteEntities(entities);

    for (auto ent : newTransformRemove) {
        ctx->engine->DeleteComponents<NewTransform2D>(ent);
    }
}
};
