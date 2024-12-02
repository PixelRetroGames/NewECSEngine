#include <iostream>

#define DLL_EXPORT

#include "script.h"
#include "game.h"

extern "C"
{

    class Script1 : public DynamicLoader::DynamicScriptInterface {
      private:
        GameEngine::GameContext *ctx;
      public:

        Script1(GameEngine::GameContext *ctx) : ctx(ctx) {}

        virtual void SetUp() override {
        }

        virtual void Run() override {
            const int SCREEN_WIDTH = 1920;
            const int SCREEN_HEIGHT = 1080;
            auto window = Engine::Window("test", SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.8, Engine::WindowFlags::WINDOW_RESIZABLE);
            window.Open();
            SpaceShooter::SpaceShooterGameWithSystems(&window);
            window.Close();
        }

        virtual void TearDown() override {
        }
    };

    DECLDIR DynamicLoader::DynamicScriptInterface *CreateScript(GameEngine::GameContext *ctx) {
        return new Script1(ctx);
    }
}

