#include <iostream>

#define DLL_EXPORT

#include "script1.h"
#include "../../src/engine/texture.h"
#include "../../space_shooter_game/shared.h"

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

                auto texture = manager->Load("assets/enemy_ship.png");
                ctx->engine->AddComponent(entity, SpaceShooter::Collider(0, 0, texture->w, texture->h));
                ctx->engine->AddComponent(entity, SpaceShooter::DestroyOnCollision());

                ctx->engine->AddComponent(entity, SpaceShooter::Transform2D(i * 150, i * 150));
                ctx->engine->AddComponent(entity, SpaceShooter::Scaling(0.2, 0.2));
                ctx->engine->AddComponent(entity, SpaceShooter::Velocity2D(0, 0));


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
            for (const auto &[transform2D, renderRect, scaling] : ctx->engine->GetGroupView<Transform2D, RenderRect, Scaling>(ECS::Unused<PlayerMovement>())) {
                int dirX = 0, dirY = 0;

                if (ctx->window->GetKeystate(Engine::Scancode::SCANCODE_LEFT)) {
                    dirX = -1;
                } else if (ctx->window->GetKeystate(Engine::Scancode::SCANCODE_RIGHT)) {
                    dirX = 1;
                }

                if (ctx->window->GetKeystate(Engine::Scancode::SCANCODE_UP)) {
                    dirY = -1;
                } else if (ctx->window->GetKeystate(Engine::Scancode::SCANCODE_DOWN)) {
                    dirY = 1;
                }

                float dt = *(ctx->dt);

                transform2D.x += dirX * 1000 * dt;
                transform2D.x = std::min(transform2D.x, ctx->window->GetPosition().w - renderRect.w * scaling.x);
                transform2D.x = std::max(transform2D.x, 0.0f);

                transform2D.y += dirY * 1000 * dt;
                transform2D.y = std::min(transform2D.y, ctx->window->GetPosition().h - renderRect.h * scaling.y);
                transform2D.y = std::max(transform2D.y, 0.0f);
            }
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

