#include "game.h"

#include <typeindex>
#include <typeinfo>

#include "../src/ecs/engine.h"
#include "components.h"
#include "systems.h"

#include "../dynamicTest/dynamic_loader.h"
#include "shared.h"

namespace SpaceShooter {

/*ECSEngineClassWrapper engineWrapper;
ResourceManagerClassWrapper resourceManagerWrapper;
WindowClassWrapper windowWrapper;

GameContextWrapper gameContextWrapper(&engineWrapper, &resourceManagerWrapper, &windowWrapper);

DynamicGameContext dynamicGameContext;

ECS::Engine<ECS::InefficientEngineCore> *EngineWrapper::globalEngine;*/

void SpaceShooterGameWithSystems(Engine::Window *window) {
    SpaceShooter::window = window;
    LOG::AddLogger("debug");
    LOG_INFO("debug", "Engine info log");
    LOG_INFO("debug", "Starting loading");

    auto font = EngineWrapper::fontSharedManager->Load("consola.ttf", 20);
    auto hpFont = EngineWrapper::fontSharedManager->Load("consola.ttf", 40);

    ECS::Engine<ECS::InefficientEngineCore> *engine = new ECS::Engine(new ECS::InefficientEngineCore());
    float dt;

    DynamicLoader::DynamicScriptManager<GameContext> dynamicScriptManager;
    GameContext gameContext(&dynamicScriptManager, engine, &EngineWrapper::resourceManager, window, &dt);
    dynamicScriptManager.SetContext(&gameContext);

    /*EngineWrapper::globalEngine = engine;

    engineWrapper.SetData(engine);
    resourceManagerWrapper.SetData(&EngineWrapper::resourceManager);
    windowWrapper.SetData(window);*/

    auto ent1 = engine->CreateEntity();
    auto ent2 = engine->CreateEntity();

    {
        //auto texture = resourceManager.LoadShared<Engine::Texture>(Engine::TextureLoadArgs("alex_the_coder.png"));
        auto texture = EngineWrapper::resourceManager.GetSharedManager<Engine::Texture, Engine::TextureLoadArgs>()->Load("alex_the_coder.png");
        engine->AddComponent(ent1, texture);
        engine->AddComponent(ent1, Transform2D(0, 0));
        engine->AddComponent(ent1, RenderRect(0, 0, texture->w / 2, texture->h));
        engine->AddComponent(ent1, Collider(0, 0, texture->w / 2, texture->h));
        engine->AddComponent(ent1, Velocity2D(400, 500));
        engine->AddComponent(ent1, Scaling(1, 1));
    }

    {
        auto ttf = EngineWrapper::resourceManager.LoadUnique<Engine::Texture>(Engine::TTFTextureLoadArgs(font.get(), "ciolty", Engine::Color(255, 0, 0)));
        engine->AddComponent(ent2, Transform2D(200, 200));
        engine->AddComponent(ent2, RenderRect(0, 0, ttf->w, ttf->h));
        engine->AddComponent(ent2, Collider(0, 0, ttf->w, ttf->h));
        engine->AddComponent(ent2, Velocity2D(700, 700));
        engine->AddComponent(ent2, Scaling(1, 1));
        engine->AddComponent(ent2, std::move(ttf));
    }

    Engine::Timer timer;
    Engine::Timer physicsTimer;
    Engine::Timer fpsTimer;
    timer.Start();
    fpsTimer.Start();

    auto ship = engine->CreateEntity();
    auto enemy = engine->CreateEntity();

    {
        auto texture = EngineWrapper::textureSharedManager->Load("assets/enemy_ship.png");
        auto scale = Scaling(0.3, 0.3);
        engine->AddComponent(enemy, texture);
        engine->AddComponent(enemy, Transform2D((window->GetPosition().w - texture->w * scale.x) / 2, 0));
        engine->AddComponent(enemy, Velocity2D(200, 400));
        engine->AddComponent(enemy, RenderRect(0, 0, texture->w, texture->h));
        engine->AddComponent(enemy, Collider(0, 0, texture->w, texture->h));
        engine->AddComponent(enemy, Health(20));
        engine->AddComponent(enemy, scale);
    }

    {
        auto scale = Scaling(0.3, 0.3);
        auto texture = EngineWrapper::textureSharedManager->Load("assets/ship5.png");
        engine->AddComponent(ship, texture);
        engine->AddComponent(ship, Transform2D((window->GetPosition().w - texture->w * scale.x) / 2, window->GetPosition().h - texture->h * scale.y));
        engine->AddComponent(ship, RenderRect(0, 0, texture->w, texture->h));
        engine->AddComponent(ship, Collider(0, 0, texture->w, texture->h));
        engine->AddComponent(ship, scale);
        engine->AddComponent(ship, PlayerMovement());
        engine->AddComponent(ship, BulletShooter(Engine::Scancode::SCANCODE_SPACE));
    }

    Collider windowPos;

    {
        auto aux = window->GetPosition();
        windowPos = Collider(aux.x, aux.y, aux.w, aux.h);
    }

    {
        double colliderSize = 10;

        auto up = engine->CreateEntity();
        engine->AddComponent(up, Collider(0, 0, windowPos.w + 2 * colliderSize, colliderSize));
        engine->AddComponent(up, Transform2D(-colliderSize, -colliderSize));
        engine->AddComponent(up, ReflectCollider());

        auto down = engine->CreateEntity();
        engine->AddComponent(down, Collider(0, 0, windowPos.w + colliderSize, colliderSize));
        engine->AddComponent(down, Transform2D(-colliderSize, windowPos.h + colliderSize));
        engine->AddComponent(down, ReflectCollider());

        auto left = engine->CreateEntity();
        engine->AddComponent(left, Collider(0, 0, colliderSize, windowPos.h));
        engine->AddComponent(left, Transform2D(-colliderSize, 0));
        engine->AddComponent(left, ReflectCollider());

        auto right = engine->CreateEntity();
        engine->AddComponent(right, Collider(0, 0, colliderSize, windowPos.h));
        engine->AddComponent(right, Transform2D(windowPos.w, 0));
        engine->AddComponent(right, ReflectCollider());
    }

    auto playerMovementScript = dynamicScriptManager.Load("script1.dll", "CreatePlayerMovementScript");
    Engine::Timer timerX;

    engine->Register([&window, &dynamicScriptManager, &dt, &timerX, &playerMovementScript]() {
        if (window->GetKeystate(Engine::Scancode::SCANCODE_Q)) {
            dt = 0.1;
        }

        if (window->GetKeystate(Engine::Scancode::SCANCODE_F10) && timerX.GetTime() > 0.1) {
            LOG_INFO("debug", "sqq");
        }

        if (window->GetKeystate(Engine::Scancode::SCANCODE_F10) && timerX.GetTime() > 0.1) {
            dynamicScriptManager.UnLoadLib("script1.dll");
            //dynamicScriptManager.UnLoadLib("script2.dll");
            //playerMovementScript = dynamicScriptManager.Load("script.dll", "CreatePlayerMovementScript");
            timerX.Start();
        }

        if (window->GetKeystate(Engine::Scancode::SCANCODE_F11) && timerX.GetTime() > 0.1) {
            //dynamicScriptManager.UnLoadLib("script1.dll");
            //dynamicScriptManager.UnLoadLib("script2.dll");
            playerMovementScript = dynamicScriptManager.Load("script1.dll", "CreatePlayerMovementScript");
            timerX.Start();
        }

        if (window->GetKeystate(Engine::Scancode::SCANCODE_F1) && timerX.GetTime() > 0.1) {
            auto scriptWeak = dynamicScriptManager.Load("script1.dll", "CreateScript1");
            if (auto script = scriptWeak.lock()) {
                script->Run();
                timerX.Start();
            }
        }

        if (window->GetKeystate(Engine::Scancode::SCANCODE_F2) && timerX.GetTime() > 0.1) {
            auto scriptWeak = dynamicScriptManager.Load("script1.dll", "CreateScript2");
            if (auto script = scriptWeak.lock()) {
                script->Run();
                timerX.Start();
            }
        }

        if (window->GetKeystate(Engine::Scancode::SCANCODE_F3) && timerX.GetTime() > 0.1) {
            auto scriptWeak = dynamicScriptManager.Load("script2.dll", "CreateScript1");
            if (auto script = scriptWeak.lock()) {
                script->Run();
                timerX.Start();
            }
        }

        if (window->GetKeystate(Engine::Scancode::SCANCODE_F4) && timerX.GetTime() > 0.1) {
            auto scriptWeak = dynamicScriptManager.Load("script2.dll", "CreateScript2");
            if (auto script = scriptWeak.lock()) {
                script->Run();
                timerX.Start();
            }
        }
    });

    engine->Register([&playerMovementScript, &gameContext]() {
        if (auto script = playerMovementScript.lock()) {
            script->Run();
        } else {
            //LOG_INFO("debug", "Script unloaded");
        }
    });

    /*auto sq = dynamicScriptManager.Load("script2.dll", "CreatePlayerMovementScript");
    engine->Register([sq, &gameContext]() {
        if (auto script = sq.lock()) {
            script->Run(&gameContext, nullptr);
        } else {
            LOG_INFO("debug", "Script unloaded");
        }
    });*/

    //engine->Register(new PlayerMovementSystem(&gameContext));
    engine->Register(new BulletShooterSystem<decltype(EngineWrapper::textureSharedManager)>(window, EngineWrapper::textureSharedManager, engine));

    engine->Register(new PhysicsSystem(&gameContext, windowPos));
    engine->Register(new CollisionDetectionSystem(window, dt, engine, windowPos));
    engine->Register(new CollisionResolutionSystem(&gameContext));
    engine->Register(new PhysicsTransformUpdateSystem(&gameContext));

    engine->Register(new TextureRendererSystem(window, engine));
    engine->Register(new ColliderDebugSystem(&gameContext));
    engine->Register(new HealthRenderer(&gameContext, hpFont));


    //engine->Register(new HealthRenderer<decltype(EngineWrapper::textureSharedManager), decltype(EngineWrapper::ttfUniqueManager)>(window, EngineWrapper::textureSharedManager, EngineWrapper::ttfUniqueManager, hpFont, engine));

    //engine->Register(new BulletCollisionSystem(window, dt, engine));

    //engine->Register(new VelocityMovementSystem(window, dt, engine, windowPos));

    timerX.Start();

    /*dynamicGameContext.engine->AddFunction("AddComponent", engineWrapper.AddComponent<Transform2D>);
    dynamicGameContext.engine->AddFunction("AddComponent", engineWrapper.AddComponent<Velocity2D>);
    dynamicGameContext.engine->AddFunction("AddComponent", engineWrapper.AddComponent<RenderRect>);
    dynamicGameContext.engine->AddFunction("AddComponent", engineWrapper.AddComponent<Scaling>);
    dynamicGameContext.engine->AddFunction("AddComponent", engineWrapper.AddComponent<Engine::SharedResource<Engine::Texture>>);
    dynamicGameContext.engine->AddFunction("AddComponent", engineWrapper.AddComponent<DestroyOnCollision>);
    dynamicGameContext.engine->AddFunction("AddComponent", engineWrapper.AddComponent<Collider>);
    dynamicGameContext.engine->AddFunction("CreateEntity", engineWrapper.CreateEntity);
    dynamicGameContext.engine->AddFunction("GetGroupView", engineWrapper.GetGroupView<Transform2D, RenderRect, Scaling, PlayerMovement>);*/

    /*dynamicGameContext.resourceManager->AddFunction("GetSharedManager", resourceManagerWrapper.GetSharedManager<Engine::Texture, Engine::TextureLoadArgs>);

    dynamicGameContext.dt = &dt;
    dynamicGameContext.engine = engine;
    dynamicGameContext.window->AddFunction("GetPosition", windowWrapper.GetPosition);
    dynamicGameContext.window->AddFunction("GetKeystate", windowWrapper.GetKeystate);

    {
        auto enemy = engine->CreateEntity();
        auto texture = EngineWrapper::textureSharedManager->Load("assets/enemy_ship.png");
        auto scale = Scaling(0.2, 0.2);
        //engine->AddComponent(enemy, texture);
        dynamicGameContext.engine->AddComponent(enemy, std::string("assets/enemy_ship.png"));
        dynamicGameContext.engine->AddComponent(enemy, Transform2D(100, 0));
        dynamicGameContext.engine->AddComponent(enemy, Velocity2D(0, 1000));
        dynamicGameContext.engine->AddComponent(enemy, RenderRect(0, 0, texture->w, texture->h));
        dynamicGameContext.engine->AddComponent(enemy, scale);
    }*/

    const int TARGET_FRAMERATE = 1000;
    double timePerFrame = 1000.0 / (1.0 * TARGET_FRAMERATE);

    Engine::Timer frameComputation;

    bool paused = false;

    while (!window->GetKeystate(Engine::Scancode::SCANCODE_ESCAPE)) {
        frameComputation.Start();

        window->PumpEvents();

        dt = timer.GetTime();
        timer.Start();

        if (window->GetKeystate(Engine::Scancode::SCANCODE_P) && timerX.GetTime() > 0.1) {
            paused = !paused;
            timerX.Start();
        }

        if (!paused) {
            engine->CallAll();

            float fps = fpsTimer.GetTime();
            auto fpsTexture = EngineWrapper::ttfUniqueManager->Load(font.get(), std::to_string(fps), Engine::Color(0, 255, 0));
            window->GetRenderer()->DrawTexture(window->GetPosition().w - fpsTexture->w,
                                               0,
                                               fpsTexture.get());

            window->GetRenderer()->Present();
            window->GetRenderer()->Flush();
        }

        //LOG_INFO("debug", "%f", fps);
        fpsTimer.Start();

        double frameTime = frameComputation.GetTime();
        if (frameTime < timePerFrame) {
            SDL_Delay(timePerFrame - frameTime);
            //SDL_Delay(10000);
        }
    }

    dynamicScriptManager.UnLoadLib("script.dll");

    LOG_INFO("debug", "Finished loading");
}

}


