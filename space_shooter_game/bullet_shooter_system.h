#pragma once

#include "shared.h"

namespace SpaceShooter {

template <typename ResourceManagerTT>
class BulletShooterSystem : public ECS::SystemInterface {
private:
    Engine::Window* &window;
    ResourceManagerTT &textureSharedManager;
    ECS::Engine<ECS::InefficientEngineCore>* &engine;

public:
    BulletShooterSystem(Engine::Window* &window,
        ResourceManagerTT &textureSharedManager,
        ECS::Engine<ECS::InefficientEngineCore>* &engine):
        window(window), textureSharedManager(textureSharedManager), engine(engine) {}

    virtual void Update() override {
        auto group = engine->GetGroupView<Transform2D, BulletShooter, SharedTexturePtr, Scaling>();
        auto end = group.end();
        for (auto it = group.begin(); it != end; ++it) {
            auto [transform2D, bulletShooter, sharedTexturePtr, scaling] = it.operator->();
            auto test = sharedTexturePtr->get();
            Scaling testScaling = *scaling;
            if (!window->GetKeystate(bulletShooter->key)) {
                continue;
            }

            if (bulletShooter->timer.GetTime() < 0.3) {
                continue;
            }

            auto bullet = engine->CreateEntity();

            auto texture = textureSharedManager->Load("assets/red_bullet.png");

            engine->AddComponent(bullet, texture);
            engine->AddComponent(bullet, Transform2D(transform2D->x + test->w / 2 * testScaling.x - texture->w / 2 * 0.1, transform2D->y - texture->h * 0.1));
            engine->AddComponent(bullet, RenderRect(0, 0, texture->w, texture->h));
            engine->AddComponent(bullet, Collider(texture->w * 0.25 * 0.1, 0, texture->w * 0.5, texture->h));
            engine->AddComponent(bullet, Velocity2D(0, -1000));
            engine->AddComponent(bullet, Scaling(0.1, 0.1));
            //engine->AddComponent(bullet, DestroyOnCollision());

            bulletShooter->timer.Start();
        }
        /*for (const auto [transform2D, bulletShooter, shooterTexturePtr] : engine->GetGroupView<Transform2D, BulletShooter, SharedTexturePtr>()) {
            if (!window->GetKeystate(bulletShooter.key)) {
                continue;
            }

            if (bulletShooter.timer.GetTime() < 0.3) {
                continue;
            }

            auto bullet = engine->CreateEntity();

            auto scale = Scaling(0.3, 0.3);
            auto texture = textureSharedManager->Load("assets/red_bullet.png");

            engine->AddComponent(bullet, texture);
            engine->AddComponent(bullet, Transform2D(transform2D.x + shooterTexturePtr->get()->w / 2, transform2D.y));
            engine->AddComponent(bullet, RenderRect(0, 0, texture->w, texture->h));
            engine->AddComponent(bullet, Collider(0, 0, texture->w, texture->h));
            engine->AddComponent(bullet, Velocity2D(0, -1000));
            engine->AddComponent(bullet, Scaling(0.1, 0.1));
            engine->AddComponent(bullet, DestroyOnCollision());

            bulletShooter.timer.Start();
        }*/
    }
};

}

