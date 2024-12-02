#include <iostream>

#define DLL_EXPORT

#include "script2.h"
#include "../../src/engine/texture.h"

extern "C"
{
    using namespace SpaceShooter;

    class Script1 : public DynamicLoader::DynamicScriptInterface {
      private:
        GameContext *ctx;
      public:

        Script1(GameContext *ctx) : ctx(ctx) {}

        virtual void SetUp() override {
        }

        virtual void Run() override {
            auto manager = ctx->resourceManager->GetSharedManager<Engine::Texture, Engine::TextureLoadArgs>();

            for (int i = 0; i < 5; i++) {
                auto entity = ctx->engine->CreateEntity();

                auto texture = manager->Load("assets/enemy_ship.png");

                ctx->engine->AddComponent(entity, texture);

                ctx->engine->AddComponent(entity, SpaceShooter::Collider(0, 0, texture->w, texture->h));
                //ctx->engine->AddComponent(entity, SpaceShooter::DestroyOnCollision());

                ctx->engine->AddComponent(entity, SpaceShooter::Transform2D(texture->w * 0.4 * i, texture->h * 0.4 * i));
                ctx->engine->AddComponent(entity, SpaceShooter::Scaling(0.4, 0.4));
                ctx->engine->AddComponent(entity, SpaceShooter::Velocity2D(300, 300));
            }

            std::cout << "DLL Called!" << std::endl;
        }

        virtual void TearDown() override {
        }
    };

    class Script2 : public DynamicLoader::DynamicScriptInterface {
      private:
        std::vector<ECS::Entity> entities;
        GameContext *ctx;
      public:

        Script2(GameContext *ctx) : ctx(ctx) {}

        virtual void SetUp() override {
        }

        virtual void Run() override {
            auto manager = ctx->resourceManager->GetSharedManager<Engine::Texture, Engine::TextureLoadArgs>();

            for (int i = 0; i < 5; i++) {
                auto entity = ctx->engine->CreateEntity();
                entities.push_back(entity);

                ctx->engine->AddComponent(entity, SpaceShooter::Collider(0, 0, 80, 80));
                ctx->engine->AddComponent(entity, SpaceShooter::DestroyOnCollision());

                ctx->engine->AddComponent(entity, SpaceShooter::Transform2D(i * 150, i * 150));
                ctx->engine->AddComponent(entity, SpaceShooter::Scaling(0.2, 0.2));
                ctx->engine->AddComponent(entity, SpaceShooter::Velocity2D(0, 0));

                auto texture = manager->Load("assets/enemy_ship.png");

                ctx->engine->AddComponent(entity, texture);
            }

            std::cout << "DLL Called!" << std::endl;
        }

        virtual void TearDown() override {
            ctx->engine->DeleteEntities(entities);
        }
    };

    class PlayerMovementScript : public DynamicLoader::DynamicScriptInterface {
      private:
        GameContext *ctx;
      public:

        PlayerMovementScript(GameContext *ctx) : ctx(ctx) {}
        virtual void SetUp() override {
        }

        virtual void Run() override {
            //const auto &[transform2D, renderRect, scaling] = *(std::tuple<Transform2D&, RenderRect&, Scaling&>*)ctxCustom;

            /*Transform2D &transform2D = *(Transform2D*) ctxCustom;
            transform2D.x -= 10;
            transform2D.y -= 10;*/
            //{
            /*Transform2D transform2D;
            RenderRect renderRect;
            Scaling scaling;*/
            std::string entities;

            for (const auto &[entity, sq] : ctx->engine->GetGroupView<ECS::Entity, SpaceShooter::DestroyOnCollision>()) {
                entities += " " + std::to_string(entity);
            }

            std::cout << "DestroyOnCollision: " << entities << '\n';
        }

        virtual void TearDown() override {
        }
    };

    DECLDIR DynamicLoader::DynamicScriptInterface *CreateScript1(GameContext *ctx) {
        return new Script1(ctx);
    }

    DECLDIR DynamicLoader::DynamicScriptInterface *CreateScript2(GameContext *ctx) {
        return new Script2(ctx);
    }

    DECLDIR DynamicLoader::DynamicScriptInterface *CreatePlayerMovementScript(GameContext *ctx) {
        return new PlayerMovementScript(ctx);
    }
}

