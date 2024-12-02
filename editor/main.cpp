#include <cstdio>

#define SDL_MAIN_HANDLED

#include "../src/engine/SDL_init.h"

#include "../src/game_engine/engine.h"

#include <thread>

int main(__attribute__((unused)) int argc, __attribute__((unused)) char* args[]) {
    Engine::Init();

    auto gameEngine = GameEngine::GameEngine();

    gameEngine.Execute("Pong.dll", "CreateScript");
    //gameEngine.Execute("SpaceShooter.dll", "CreateScript");

    LOG_INFO("debug", "Textures should be deleted");
    Engine::Close();

    return 0;
}
