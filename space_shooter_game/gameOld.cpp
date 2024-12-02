#include "game.h"

#include "../src/ecs/engine.h"
#include "components.h"
#include "systems.h"

namespace SpaceShooter {

typedef Engine::SharedResource<Engine::Texture> SharedTexturePtr;
typedef Engine::UniqueResource<Engine::Texture> UniqueTexturePtr;

Engine::Window *window;

Engine::Texture *LoadTexture(Engine::TextureLoadArgs args) {
    return window->GetRenderer()->LoadTexture(args.path);
}

void DeleteTexture(Engine::Texture *texture) {
    texture->Clear();
}

typedef Engine::SharedResource<Engine::Font> FontPtr;

Engine::Font *LoadFont(Engine::FontLoadArgs args) {
    return Engine::LoadFont(args.path, args.size);
}

void DeleteFont(Engine::Font *font) {
    font->Clear();
}

Engine::Texture *CreateTTF(Engine::TTFTextureLoadArgs args) {
    return window->GetRenderer()->CreateTTFTexture(args.font, args.text, args.color);
}

void RenderScene(ECS::Engine<ECS::InefficientEngineCore>* &engine, Engine::Window* &window) {
    window->GetRenderer()->Flush();

    /*for (auto *[transform2D, renderRect, scaling, sharedTexturePtr, uniqueTexturePtr] :
            engine->GetGroupView<Transform2D, RenderRect, Scaling>(ECS::Optional<SharedTexturePtr, UniqueTexturePtr>())) {*/
    auto group = engine->GetGroupView<Transform2D, RenderRect, Scaling>(ECS::Optional<SharedTexturePtr, UniqueTexturePtr>());
    auto end = group.end();
    for (auto it = group.begin(); it != end; ++it) {
        auto [transform2D, renderRect, scaling, sharedTexturePtr, uniqueTexturePtr] = it.operator->();
        Engine::Texture *texture = nullptr;
        if (sharedTexturePtr) {
            texture = sharedTexturePtr->get();
        } else if (uniqueTexturePtr) {
            texture = uniqueTexturePtr->get();
        } else {
            continue;
        }

        window->GetRenderer()->DrawTexture(*renderRect,
                                           texture,
                                           Engine::Rectangle(transform2D->x, transform2D->y, scaling->x * (float)renderRect->w, scaling->y * (float)renderRect->h));
    }
}

#define _Args(...) __VA_ARGS__
#define STRIP_PARENS(X) X

#define REGISTER(engine, capture, _view, lambda) \
engine->Register([STRIP_PARENS( _Args capture )] () {\
                 auto view = STRIP_PARENS (_Args _view ) ;\
                 STRIP_PARENS ( _Args lambda )})

void SpaceShooterGameWithSystems(Engine::Window *window) {
    SpaceShooter::window = window;
    LOG::AddLogger("debug");
    LOG_INFO("debug", "Engine info log");
    LOG_INFO("debug", "Starting loading");

    auto textureSharedManager = Engine::MakeSharedResourceManager(LoadTexture, DeleteTexture);
    auto ttfUniqueManager = Engine::MakeUniqueResourceManager(CreateTTF, DeleteTexture);

    auto fontSharedManager = Engine::MakeSharedResourceManager(LoadFont, DeleteFont);
    auto font = fontSharedManager->Load("consola.ttf", 20);

    ECS::Engine<ECS::InefficientEngineCore> *engine = new ECS::Engine(new ECS::InefficientEngineCore());

    auto ent1 = engine->CreateEntity();
    auto ent2 = engine->CreateEntity();

    auto texture = textureSharedManager->Load("alex_the_coder.png");
    //engine->AddComponent(ent1, texture);
    engine->AddComponent(ent1, Transform2D(0, 0));
    engine->AddComponent(ent1, RenderRect(0, 0, texture->w / 2, texture->h));
    engine->AddComponent(ent1, Collider(0, 0, texture->w / 2, texture->h));
    engine->AddComponent(ent1, Velocity2D(400, 500));
    engine->AddComponent(ent1, Scaling(1, 1));

    //texture = textureSharedManager->Load("timy_skin.png");
    {
        auto ttf = ttfUniqueManager->Load(font.get(), "ciolty", Engine::Color(255, 0, 0));
        engine->AddComponent(ent2, Transform2D(200, 200));
        engine->AddComponent(ent2, RenderRect(0, 0, ttf->w, ttf->h));
        engine->AddComponent(ent2, Collider(0, 0, ttf->w, ttf->h));
        engine->AddComponent(ent2, Velocity2D(500, 1000));
        engine->AddComponent(ent2, Scaling(1, 1));
        engine->AddComponent(ent2, std::move(ttf));
    }

    int numEntities = 2;

    Engine::Timer timer;
    Engine::Timer physicsTimer;
    Engine::Timer fpsTimer;
    timer.Start();
    fpsTimer.Start();

    auto ship = engine->CreateEntity();
    //auto keepAlive = textureSharedManager->Load("assets/ship5.png");

    {
        auto scale = Scaling(0.3, 0.3);
        auto texture = textureSharedManager->Load("assets/ship5.png");
        engine->AddComponent(ship, texture);
        engine->AddComponent(ship, Transform2D((window->GetPosition().w - texture->w * scale.x) / 2, window->GetPosition().h - texture->h * scale.y));
        engine->AddComponent(ship, RenderRect(0, 0, texture->w, texture->h));
        engine->AddComponent(ship, scale);
        engine->AddComponent(ship, PlayerMovement());
        engine->AddComponent(ship, BulletShooter(Engine::Scancode::SCANCODE_SPACE));
    }
    //engine->AddComponent(ent1, Collider(0, 0, texture->w / 2, texture->h));
    //engine->AddComponent(ent1, Velocity2D(400, 500));

    auto aux = window->GetPosition();
    auto windowPos = Collider(aux.x, aux.y, aux.w, aux.h);
    bool paused = false;
    float dt;

    /*engine->Register(ECS::System([&window, &dt, &engine]() {
        for (const auto &[transform2D, renderRect, scaling] : engine->GetGroupView<Transform2D, RenderRect, Scaling>(ECS::Unused<PlayerMovement>())) {
            int dirX = 0, dirY = 0;
            if (window->GetKeystate(Engine::Scancode::SCANCODE_LEFT)) {
                dirX = -1;
            } else if (window->GetKeystate(Engine::Scancode::SCANCODE_RIGHT)) {
                dirX = 1;
            }

            if (window->GetKeystate(Engine::Scancode::SCANCODE_UP)) {
                dirY = -1;
            } else if (window->GetKeystate(Engine::Scancode::SCANCODE_DOWN)) {
                dirY = 1;
            }

            transform2D.x += dirX * 1500 * dt;
            transform2D.x = std::min(transform2D.x, window->GetPosition().w - renderRect.w * scaling.x);
            transform2D.x = std::max(transform2D.x, 0.0f);

            transform2D.y += dirY * 1500 * dt;
            transform2D.y = std::min(transform2D.y, window->GetPosition().h - renderRect.h * scaling.y);
            transform2D.y = std::max(transform2D.y, 0.0f);
        }
    }, [] () {
        return ECS::ComponentGroupMask(ECS::ComponentPack<Transform2D, RenderRect, Scaling>(), ECS::Unused<PlayerMovement>(), ECS::Optional<>());
    }));*/

    engine->Register(new PlayerMovementSystem(window, dt, engine));

    /*REGISTER(engine, (&window, &dt, &engine), (engine->GetGroupView<Transform2D, RenderRect, Scaling>(ECS::Unused<PlayerMovement>())), (
        for (const auto &[transform2D, renderRect, scaling] : view) {
            int dirX = 0, dirY = 0;
            if (window->GetKeystate(Engine::Scancode::SCANCODE_LEFT)) {
                dirX = -1;
            } else if (window->GetKeystate(Engine::Scancode::SCANCODE_RIGHT)) {
                dirX = 1;
            }

            if (window->GetKeystate(Engine::Scancode::SCANCODE_UP)) {
                dirY = -1;
            } else if (window->GetKeystate(Engine::Scancode::SCANCODE_DOWN)) {
                dirY = 1;
            }

            transform2D.x += dirX * 1500 * dt;
            transform2D.x = std::min(transform2D.x, window->GetPosition().w - renderRect.w * scaling.x);
            transform2D.x = std::max(transform2D.x, 0.0f);

            transform2D.y += dirY * 1500 * dt;
            transform2D.y = std::min(transform2D.y, window->GetPosition().h - renderRect.h * scaling.y);
            transform2D.y = std::max(transform2D.y, 0.0f);
        }
    ));*/

    engine->Register(ECS::System([&engine, &dt, &paused, &windowPos]() {
        if (!paused) {
            for (const auto &[transform2D, velocity2D, colliderOriginal, scaling] : engine->GetGroupView<Transform2D, Velocity2D, Collider, Scaling>()) {
                //auto  = *it;
                Collider collider = colliderOriginal;
                Transform2D oldTransform = transform2D;

                transform2D.x += dt * velocity2D.x;
                transform2D.y += dt * velocity2D.y;

                collider.x += transform2D.x;
                collider.y += transform2D.y;

                collider.w *= scaling.x;
                collider.h *= scaling.y;

                bool collidedX = false;
                bool collidedY = false;
                float factor = -1;

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

                /*if (collidedX || collidedY) {
                    DestroyOnCollision
                }*/
            }
        }
    }, [](){
        return ECS::ComponentGroupMask(ECS::ComponentPack<Transform2D, Velocity2D, Collider, Scaling>(), ECS::Unused<>(), ECS::Optional<>());
    }));

    engine->Register(ECS::System([&engine, &window]() {
        RenderScene(engine, window);
    }, [](){
        return ECS::ComponentGroupMask(ECS::ComponentPack<Transform2D, Velocity2D, Collider, Scaling>(), ECS::Unused<>(), ECS::Optional<>());
    }));

    engine->Register([&engine, &textureSharedManager, &window] () {
        for (const auto &[transform2D, bulletShooter] : engine->GetGroupView<Transform2D, BulletShooter>()) {
            if (!window->GetKeystate(bulletShooter.key)) {
                continue;
            }

            if (bulletShooter.timer.GetTime() < 0.1) {
                continue;
            }

            auto bullet = engine->CreateEntity();

            auto scale = Scaling(0.3, 0.3);
            auto texture = textureSharedManager->Load("assets/red_bullet.png");

            engine->AddComponent(bullet, texture);
            engine->AddComponent(bullet, transform2D);
            engine->AddComponent(bullet, RenderRect(0, 0, texture->w, texture->h));
            engine->AddComponent(bullet, Collider(0, 0, texture->w, texture->h));
            engine->AddComponent(bullet, Velocity2D(0, -1000));
            engine->AddComponent(bullet, Scaling(0.1, 0.1));
            engine->AddComponent(bullet, DestroyOnCollision());

            bulletShooter.timer.Start();
        }
    });

    while (!window->GetKeystate(Engine::Scancode::SCANCODE_ESCAPE)) {
        dt = timer.GetTime();
        timer.Start();

        physicsTimer.Start();

        /*if (window->GetKeystate(Engine::Scancode::SCANCODE_SPACE)) {
            paused = !paused;
            SDL_Delay(1000);
            timer.Start();
        }*/

        if (window->GetKeystate(Engine::Scancode::SCANCODE_Q)) {
            dt = 0.1;
        }

        if (window->GetKeystate(Engine::Scancode::SCANCODE_RETURN)) {
            int multiplier = 1;
            if (window->GetKeystate(Engine::Scancode::SCANCODE_LSHIFT)) {
                multiplier = 10;
            }
            if (window->GetKeystate(Engine::Scancode::SCANCODE_LCTRL)) {
                multiplier *= 10;
            }

            while (multiplier--) {
                numEntities++;
                auto ent1 = engine->CreateEntity();
                auto texture = textureSharedManager->Load("assets/ship5.png");
                engine->AddComponent(ent1, texture);
                engine->AddComponent(ent1, Transform2D(multiplier, multiplier));
                engine->AddComponent(ent1, RenderRect(0, 0, texture->w / 2, texture->h));
                engine->AddComponent(ent1, Collider(0, 0, texture->w / 2, texture->h));
                engine->AddComponent(ent1, Velocity2D(500, 500));
                engine->AddComponent(ent1, Scaling(0.5, 0.5));
            }
        }

        engine->CallAll();

        float physicsTime = physicsTimer.GetTime();

        float fps = fpsTimer.GetTime();
        auto fpsTexture = ttfUniqueManager->Load(font.get(), std::to_string(fps), Engine::Color(0, 255, 0));
        window->GetRenderer()->DrawTexture(window->GetPosition().w - fpsTexture->w,
                                           0,
                                           fpsTexture.get());

        auto numEntityTexture = ttfUniqueManager->Load(font.get(), std::to_string(numEntities), Engine::Color(255, 0, 0));
        window->GetRenderer()->DrawTexture(window->GetPosition().w - numEntityTexture->w,
                                           fpsTexture->h,
                                           numEntityTexture.get());

        auto sq = ttfUniqueManager->Load(font.get(), std::to_string(physicsTime), Engine::Color(0, 0, 255));
        window->GetRenderer()->DrawTexture(window->GetPosition().w - sq->w,
                                           fpsTexture->h + numEntityTexture->h,
                                           sq.get());

        window->GetRenderer()->Present();

        //LOG_INFO("debug", "%f", fps);
        fpsTimer.Start();
        window->PumpEvents();
    }


    LOG_INFO("debug", "Finished loading");
}

}


