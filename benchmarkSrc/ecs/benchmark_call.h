#pragma once

#include <benchmark/benchmark.h>

#include "../../src/ecs/engine.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct Transform2D {
    float x, y;

    Transform2D(float x, float y) : x(x), y(y) {}
};

struct RenderRect : public Engine::Rectangle {
    template <typename... T>
    RenderRect(T... args) : Rectangle(args...) {
    }
};

struct Collider : public Engine::Rectangle {
    template <typename... T>
    Collider(T... args) : Rectangle(args...) {
    }
};

struct Velocity2D {
    float x, y;

    Velocity2D(float x, float y) : x(x), y(y) {}
};

template <typename T>
void ECSSetup(int numEntities, ECS::Engine<T> *engine) {

    ECS::Entity entity;

    for (int i = 0; i < numEntities; i++) {
        entity = engine->CreateEntity();

        engine->AddComponent(entity, Transform2D(0, 0));
        engine->AddComponent(entity, Collider(0, 0, 5, 5));
        engine->AddComponent(entity, Velocity2D(1000, 1000));
    }
}

template <typename T>
void ECSGameLoop(int numEntities, ECS::Engine<T> *engine) {
    {
        engine->EntitiesCallFor(
            ECS::CreateMask<Transform2D, Velocity2D, Collider>(),
        [](std::vector<ECS::Entity> entities, decltype(engine) engine) {
            auto windowPos = Engine::Rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

            float dt = 0.01;

            for (const auto &[transform2D, velocity2D, colliderOriginal] : engine->template GetGroupView<Transform2D, Velocity2D, Collider>()) {
                Collider collider = colliderOriginal;

                transform2D.x += dt * velocity2D.x;
                transform2D.y += dt * velocity2D.y;

                collider.x += transform2D.x;
                collider.y += transform2D.y;

                bool collided = false;
                float factor = -1.1;

                if (collider.x + collider.w > windowPos.w) {
                    velocity2D.x *= factor;
                    collided = true;
                }

                if (collider.x < 0) {
                    velocity2D.x *= factor;
                    collided = true;
                }

                if (collider.y + collider.h > windowPos.h) {
                    velocity2D.y *= factor;
                    collided = true;
                }

                if (collider.y < 0) {
                    velocity2D.y *= factor;
                    collided = true;
                }

                if (std::abs(velocity2D.x) > 1500) {
                    velocity2D.x = velocity2D.x < 0 ? -1500 : 1500;
                }

                if (std::abs(velocity2D.y) > 1500) {
                    velocity2D.y = velocity2D.y < 0 ? -1500 : 1500;
                }

                if (collided) {
                    transform2D.x += dt * velocity2D.x;
                    transform2D.y += dt * velocity2D.y;
                }
            }
        });
    }
}

static void BM_ECSCall(benchmark::State &state) {
    int numEntities = state.range(0);

    auto engine = new ECS::Engine(new ECS::DefaultEngineCore());

    ECSSetup(numEntities, engine);

    for (auto _ : state) {
        ECSGameLoop(numEntities, engine);
    }
}

//BENCHMARK(BM_ECSCall)->RangeMultiplier(2)->Range(8, 8 << 6)->MinTime(2);

///
template <typename T>
void ECSGameLoop2(int numEntities, ECS::Engine<T> *engine) {
    {
        auto windowPos = Engine::Rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        float dt = 0.01;

        for (const auto &[transform2D, velocity2D, colliderOriginal] : engine->template GetGroupView<Transform2D, Velocity2D, Collider>()) {
            Collider collider = colliderOriginal;

            transform2D.x += dt * velocity2D.x;
            transform2D.y += dt * velocity2D.y;

            collider.x += transform2D.x;
            collider.y += transform2D.y;

            bool collided = false;
            float factor = -1.1;

            if (collider.x + collider.w > windowPos.w) {
                velocity2D.x *= factor;
                collided = true;
            }

            if (collider.x < 0) {
                velocity2D.x *= factor;
                collided = true;
            }

            if (collider.y + collider.h > windowPos.h) {
                velocity2D.y *= factor;
                collided = true;
            }

            if (collider.y < 0) {
                velocity2D.y *= factor;
                collided = true;
            }

            if (std::abs(velocity2D.x) > 1500) {
                velocity2D.x = velocity2D.x < 0 ? -1500 : 1500;
            }

            if (std::abs(velocity2D.y) > 1500) {
                velocity2D.y = velocity2D.y < 0 ? -1500 : 1500;
            }

            if (collided) {
                transform2D.x += dt * velocity2D.x;
                transform2D.y += dt * velocity2D.y;
            }
        }
    }
}

static void BM_ECSCall2(benchmark::State &state) {
    int numEntities = state.range(0);

    auto engine = new ECS::Engine(new ECS::DefaultEngineCore());

    ECSSetup(numEntities, engine);

    for (auto _ : state) {
        ECSGameLoop2(numEntities, engine);
    }
}

//BENCHMARK(BM_ECSCall2)->RangeMultiplier(2)->Range(8, 8 << 6)->MinTime(2);
