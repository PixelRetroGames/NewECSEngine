#define DLL_EXPORT

#include "pong.h"
#include "engine/texture.h"
//#include "systems.h"
#include "components.h"

#include "biecs/biecs.h"
#include <gtc/constants.hpp>

extern "C"
{

    class PongScript : public DynamicLoader::DynamicScriptInterface {
      private:
        GameEngine::GameContext *ctx;
      public:

        PongScript(GameEngine::GameContext *ctx) : ctx(ctx) {}

        virtual void SetUp() override {
        }

        virtual void Run() override {
            RunPongGame();
        }

        virtual void TearDown() override {
        }
    };

    DECLDIR DynamicLoader::DynamicScriptInterface *CreateScript(GameEngine::GameContext *ctx) {
        return new PongScript(ctx);
    }
}

using namespace BIECS;
using namespace Pong;

void RunPongGame() {
    LOG::AddLogger("debug");
    LOG_INFO("debug", "Loading pong");

    const int SCREEN_WIDTH = 1200;
    const int SCREEN_HEIGHT = 600;
    auto window = Engine::Window("Pong", SCREEN_WIDTH, SCREEN_HEIGHT);
    ::window = &window;
    window.Open();

    ECS::Engine<ECS::DefaultEngineCore> *engine = new ECS::Engine(new ECS::DefaultEngineCore());
    float dt;

    static auto textureSharedManager = Engine::MakeSharedResourceManager(LoadTexture, DeleteTexture);
    static auto ttfUniqueManager = Engine::MakeUniqueResourceManager(CreateTTF, DeleteTexture);

    static auto fontSharedManager = Engine::MakeSharedResourceManager(LoadFont, DeleteFont);
    static ResourceManagerT resourceManager = CreateResourceManager(Engine::UniqueResManagerGroup(ttfUniqueManager),
            Engine::SharedResManagerGroup(textureSharedManager, fontSharedManager));

    GameContext gameContext(engine, &resourceManager, &window, &dt);
    GameContext *ctx = &gameContext;

    // Systems registering

    // Player movement
    engine->Register([&gameContext]() {
        const float movementSpeed = 1500;

        for (auto [velocity, playerMovement] : gameContext.engine->GetGroupView<Velocity2D, Pong::PlayerMovement>()) {
            int direction = 0;

            if (gameContext.window->GetKeystate(playerMovement.keyUp)) {
                direction--;
            }

            if (gameContext.window->GetKeystate(playerMovement.keyDown)) {
                direction++;
            }

            velocity = direction * movementSpeed * Velocity2D(0, 1);
        }
    });

    engine->Register([ctx]() {
        for (auto [entity, transform2D, velocity] : ctx->engine->GetGroupView<ECS::Entity, Transform2D, Velocity2D>()) {
            //transform2D += (*ctx->dt) * velocity;

            NewTransform2D newTransform = transform2D;
            newTransform += (*ctx->dt) * velocity;
            ctx->engine->AddComponent(entity, newTransform);
        }
    });

    std::vector<BIECS::CollisionFilter> filters;

    filters.push_back([](ECS::Entity collisionEnt, const Collision & collision, GameContext * ctx) {
        auto ent1 = collision.entities[0];
        auto ent2 = collision.entities[1];

        if (ctx->engine->GetComponent<PlayerMovement>(ent1) || ctx->engine->GetComponent<PlayerMovement>(ent2)) {
            ctx->engine->AddComponent(collisionEnt, PlayerCollision());
        }
    });

    filters.push_back([](ECS::Entity collisionEnt, const Collision & collision, GameContext * ctx) {
        auto ent1 = collision.entities[0];
        auto ent2 = collision.entities[1];

        if (ctx->engine->GetComponent<Wall>(ent1) && ctx->engine->GetComponent<Wall>(ent2)) {
            return;
        }

        if (ctx->engine->GetComponent<Wall>(ent1) || ctx->engine->GetComponent<Wall>(ent2)) {
            ctx->engine->AddComponent(collisionEnt, WallCollision());
        }
    });

    filters.push_back([](ECS::Entity collisionEnt, const Collision & collision, GameContext * ctx) {
        auto ent1 = collision.entities[0];
        auto ent2 = collision.entities[1];

        if (ctx->engine->GetComponent<Exit>(ent1) || ctx->engine->GetComponent<Exit>(ent2)) {
            ctx->engine->AddComponent(collisionEnt, ExitCollision());
        }
    });

    engine->Register(new BIECS::CollisionDetectionSystem(&gameContext, filters));

    engine->Register([&ctx]() {
        const float ballVelocity = 1500;
        const float maxAngle = glm::pi<float>() * 0.3f;

        for (auto [collision] : ctx->engine->GetGroupView<Collision>(ECS::Unused<PlayerCollision>())) {
            auto player = collision.entities[0];
            auto ball = collision.entities[1];

            if (ctx->engine->GetComponent<PlayerMovement>(ball)) {
                std::swap(player, ball);
            }

            auto ballVelocityPtr = ctx->engine->GetComponent<Velocity2D>(ball);

            if (!ballVelocityPtr) {
                continue;
            }

            Collider ballCollider = *ctx->engine->GetComponent<Collider>(ball);
            {
                auto ballTransform = *ctx->engine->GetComponent<Transform2D>(ball);
                ballCollider.x += ballTransform.x;
                ballCollider.y += ballTransform.y;

                auto ballScalingPtr = ctx->engine->GetComponent<Scaling>(ball);

                if (ballScalingPtr) {
                    ballCollider.w *= ballScalingPtr->x;
                    ballCollider.h *= ballScalingPtr->y;
                }
            }

            Collider playerCollider = *ctx->engine->GetComponent<Collider>(player);
            {
                auto playerTransform = *ctx->engine->GetComponent<Transform2D>(player);
                playerCollider.x += playerTransform.x;
                playerCollider.y += playerTransform.y;

                auto playerScalingPtr = ctx->engine->GetComponent<Scaling>(player);

                if (playerScalingPtr) {
                    playerCollider.w *= playerScalingPtr->x;
                    playerCollider.h *= playerScalingPtr->y;
                }
            }

            Transform2D ballCenter = Transform2D(ballCollider.x, ballCollider.y) + Transform2D(ballCollider.w / 2.0f, ballCollider.h / 2.0f);
            Transform2D playerCenter = Transform2D(playerCollider.x, playerCollider.y) + Transform2D(playerCollider.w / 2.0f, playerCollider.h / 2.0f);

            float angle = (ballCenter.y - playerCenter.y) / (playerCollider.h / 2.0f) * maxAngle;

            float xDirection = -glm::sign(ballVelocityPtr->x);

            *ballVelocityPtr = glm::normalize(Velocity2D(xDirection * glm::cos(angle), glm::sin(angle))) * ballVelocity;

            ctx->engine->DeleteComponents<NewTransform2D>(ball);
        }
    });

    engine->Register([&ctx]() {
        for (auto [collision] : ctx->engine->GetGroupView<Collision>(ECS::Unused<WallCollision>())) {
            auto ent1 = collision.entities[0];
            auto ent2 = collision.entities[1];

            auto vel1 = ctx->engine->GetComponent<Velocity2D>(collision.entities[0]);
            auto vel2 = ctx->engine->GetComponent<Velocity2D>(collision.entities[1]);

            Transform2D wallPos;

            if (ctx->engine->GetComponent<Wall>(ent1)) {
                wallPos = *ctx->engine->GetComponent<Transform2D>(ent1);
                std::swap(ent1, ent2);
                std::swap(vel1, vel2);
            } else {
                wallPos = *ctx->engine->GetComponent<Transform2D>(ent2);
            }

            if (!vel1) {
                continue;
            }

            vel1->y *= -1.0f;
            ctx->engine->DeleteComponents<NewTransform2D>(ent1);
        }
    });

    const float ballStartingSpeed = 1000;

    engine->Register([&ctx, &ballStartingSpeed]() {
        for (auto [collision] : ctx->engine->GetGroupView<Collision>(ECS::Unused<ExitCollision>())) {
            auto ent1 = collision.entities[0];
            auto ent2 = collision.entities[1];

            auto transform1 = ctx->engine->GetComponent<NewTransform2D>(collision.entities[0]);
            auto transform2 = ctx->engine->GetComponent<NewTransform2D>(collision.entities[1]);

            int playerScoreID = 0;

            auto exitComponent = ctx->engine->GetComponent<Exit>(ent1);

            if (exitComponent) {
                std::swap(ent1, ent2);
                std::swap(transform1, transform2);
            } else {
                exitComponent = ctx->engine->GetComponent<Exit>(ent2);
            }

            playerScoreID = exitComponent->playerScoreID;

            if (!transform1) {
                continue;
            }

            auto texture = *ctx->engine->GetComponent<SharedTexturePtr>(ent1);
            auto scaling = ctx->engine->GetComponent<Scaling>(ent1);

            // Move ball to center
            *transform1 = NewTransform2D((ctx->window->GetPosition().w - texture->w * scaling->x) / 2, (ctx->window->GetPosition().h - texture->h * scaling->y) / 2);
            auto velocity = ctx->engine->GetComponent<Velocity2D>(ent1);
            *velocity = glm::normalize(*velocity) * ballStartingSpeed;

            // Update the score
            for (auto [scoreBoard] : ctx->engine->GetGroupView<ScoreBoard>()) {
                scoreBoard.scores[playerScoreID]++;
                scoreBoard.dirty = true;
            }
        }
    });

    engine->Register([&ctx]() {
        static auto font = ctx->resourceManager->LoadShared<Engine::Font>(Engine::FontLoadArgs("consola.ttf", 25));
        static auto ttfManager = ctx->resourceManager->GetUniqueManager<Engine::Texture, Engine::TTFTextureLoadArgs>();

        for (auto [entity, scoreBoard, transform2, scaling, centered, texture] : ctx->engine->GetGroupView<ECS::Entity, ScoreBoard, Transform2D, Scaling, Centered>(ECS::Optional<UniqueTexturePtr>())) {
            if (scoreBoard.dirty) {
                auto ttf = ttfManager->Load(font.get(), std::to_string(scoreBoard.scores[0]) + " - " + std::to_string(scoreBoard.scores[1]), Engine::Color(255, 0, 0));

                transform2 = centered.anchor - scaling * glm::vec2(ttf->w, ttf->h) / 2.0f;

                ctx->engine->DeleteComponents<UniqueTexturePtr>(entity);
                ctx->engine->AddComponent(entity, std::move(ttf));

                scoreBoard.dirty = false;
            }
        }
    });

    engine->Register(new BIECS::CollisionClearSystem(&gameContext));

    engine->Register([ctx]() {
        for (auto [entity, transform2D, newTransform] : ctx->engine->GetGroupView<ECS::Entity, Transform2D, NewTransform2D>()) {
            transform2D = newTransform;
            ctx->engine->DeleteComponents<NewTransform2D>(entity);
        }
    });

    engine->Register(new BIECS::TextureRendererSystem(&gameContext));

    // Set entities and components

    // ScoreBoard
    {
        auto entity = engine->CreateEntity();
        engine->AddComponent(entity, Transform2D(window.GetPosition().w / 2, 0));
        engine->AddComponent(entity, Centered(Transform2D(window.GetPosition().w / 2, 20)));
        engine->AddComponent(entity, Scaling(1, 1));
        engine->AddComponent(entity, ScoreBoard());
    }

    // Right player
    {
        auto player = engine->CreateEntity();
        auto texture = textureSharedManager->Load("samples/pong/assets/player.png");
        auto scaling = Scaling(1, 15);

        engine->AddComponent(player, Transform2D(window.GetPosition().w - texture->w * scaling.x - 10, window.GetPosition().h / 2));
        engine->AddComponent(player, std::move(texture));
        engine->AddComponent(player, scaling);
        engine->AddComponent(player, Velocity2D(0, 0));
        engine->AddComponent(player, VelocityMoved());
        engine->AddComponent(player, Collider(0, 0, 2, texture->h));
        engine->AddComponent(player, Pong::PlayerMovement(Engine::Scancode::SCANCODE_UP, Engine::Scancode::SCANCODE_DOWN));
    }

    // Left player
    {
        auto player = engine->CreateEntity();
        auto texture = textureSharedManager->Load("samples/pong/assets/player.png");
        auto scaling = Scaling(1, 15);

        engine->AddComponent(player, Transform2D(10, window.GetPosition().h / 2));
        engine->AddComponent(player, std::move(texture));
        engine->AddComponent(player, scaling);
        engine->AddComponent(player, Velocity2D(0, 0));
        engine->AddComponent(player, VelocityMoved());
        engine->AddComponent(player, Collider(0, 0, 2, texture->h));
        engine->AddComponent(player, Pong::PlayerMovement(Engine::Scancode::SCANCODE_W, Engine::Scancode::SCANCODE_S));
    }

    // Ball
    {
        auto ball = engine->CreateEntity();
        auto texture = textureSharedManager->Load("samples/pong/assets/player.png");
        auto scaling = Scaling(1, 1);

        engine->AddComponent(ball, Transform2D((window.GetPosition().w - texture->w * scaling.x) / 2, (window.GetPosition().h - texture->h * scaling.y) / 2));
        engine->AddComponent(ball, std::move(texture));
        engine->AddComponent(ball, scaling);
        engine->AddComponent(ball, VelocityMoved());
        engine->AddComponent(ball, Velocity2D(glm::normalize(Velocity2D(1, 1)) * ballStartingSpeed));
        engine->AddComponent(ball, Collider(0, 0, texture->w, texture->h));
    }

    // Screen edges colliders
    {
        double colliderSize = 10;
        auto windowPos = window.GetPosition();

        auto up = engine->CreateEntity();
        engine->AddComponent(up, Collider(0, 0, windowPos.w + 2 * colliderSize, colliderSize));
        engine->AddComponent(up, Transform2D(-colliderSize, -colliderSize));
        engine->AddComponent(up, Wall());

        auto down = engine->CreateEntity();
        engine->AddComponent(down, Collider(0, 0, windowPos.w + colliderSize, colliderSize));
        engine->AddComponent(down, Transform2D(-colliderSize, windowPos.h + colliderSize));
        engine->AddComponent(down, Wall());

        auto left = engine->CreateEntity();
        engine->AddComponent(left, Collider(0, 0, colliderSize, windowPos.h));
        engine->AddComponent(left, Transform2D(-colliderSize, 0));
        engine->AddComponent(left, Exit(1));

        auto right = engine->CreateEntity();
        engine->AddComponent(right, Collider(0, 0, colliderSize, windowPos.h));
        engine->AddComponent(right, Transform2D(windowPos.w, 0));
        engine->AddComponent(right, Exit(0));
    }

    const int TARGET_FRAMERATE = 1000;
    double timePerFrame = 1000.0 / (1.0 * TARGET_FRAMERATE);

    Engine::Timer frameComputation, timer;

    while (!window.GetKeystate(Engine::Scancode::SCANCODE_ESCAPE)) {
        frameComputation.Start();

        window.PumpEvents();

        dt = timer.GetTime();
        timer.Start();

        engine->CallAll();
        window.GetRenderer()->Present();
        window.GetRenderer()->Flush();

        double frameTime = frameComputation.GetTime();

        if (frameTime < timePerFrame) {
            SDL_Delay(timePerFrame - frameTime);
        }
    }

    LOG_INFO("debug", "Finished loading");

    window.Close();
}
