#pragma once

#include "../src/engine/resource_manager.h"
#include "../src/ecs/engine.h"
#include "../src/engine/scancodes.h"
#include <string>
#include <sstream>
#include <iomanip>

#include "glm.hpp"

namespace SpaceShooter {

struct Transform2D : public glm::vec2 {
    Transform2D() : glm::vec2(0, 0) {}

    Transform2D(float x, float y) : glm::vec2(x, y) {}

    Transform2D(const glm::vec2 &other) {
        *((glm::vec2*) this) = other;
    }

    operator glm::vec2() {
        return *this;
    }
};

struct Scaling : public Transform2D {
    template <typename... T>
    Scaling(T... args) : Transform2D(args...) {
    }

    Scaling() : Transform2D(0, 0) {}
};

struct NewTransform2D : public Transform2D {
    template <typename... T>
    NewTransform2D(T... args) : Transform2D(args...) {
    }
};

struct RenderRect : public Engine::Rectangle {
    template <typename... T>
    RenderRect(T... args) : Rectangle(args...) {
    }
};

struct Collider : public Engine::Rectangle {
    //float x, y, w, h;
    Collider(float x, float y, float w, float h) : Engine::Rectangle(x, y, w, h) {}
    Collider() : Engine::Rectangle() {}
};

struct Velocity2D : public Transform2D {
    template <typename... T>
    Velocity2D(T... args) : Transform2D(args...) {
    }

    std::string to_string() {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(3) << x << ", " << y;
        return stream.str();
    }
};

struct Rotation2D {
    float angle;
};

struct PlayerMovement {};

struct BulletShooter {
    Engine::Timer timer;
    Engine::Scancode key;

    BulletShooter(Engine::Scancode key) : key(key) {}
};

struct Health {
    int hp;

    Health(int hp) : hp(hp) {}
    operator int() {
        return hp;
    }
};

struct Colliding {
    ECS::Entity other;

    Colliding(ECS::Entity other) : other(other) {}
};

struct Collision {
    ECS::Entity entities[2];
    Collision(ECS::Entity ent1, ECS::Entity ent2) : entities{ent1, ent2} {}
};

struct ReflectCollider {};

struct DestroyOnCollision {};

struct Rigidbody {};

}
