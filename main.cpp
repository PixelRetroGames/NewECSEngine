/*#include <cstdio>

#define SDL_MAIN_HANDLED

#include "src/engine/SDL_init.h"
#include "src/engine/surface_factory.h"
#include "src/engine/window.h"

#include "src/engine/resource_manager.h"

#include "src/ecs/engine.h"

#include "src/ecs/scene_loader/scene_loader.h"

#include "src/game_engine/engine.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

typedef Engine::SharedResource<Engine::Texture> SharedTexturePtr;
typedef Engine::UniqueResource<Engine::Texture> UniqueTexturePtr;

Engine::Window *window;

#include "space_shooter_game/game.h"

int main(__attribute__((unused)) int argc, __attribute__((unused)) char* args[]) {

    /*ECSScene::SceneLoader sceneLoader;

    sceneLoader.Load("sample.json");


    //return 0;
    Engine::Init();

    auto gameEngine = GameEngine::GameEngine();

    SpaceShooter::SpaceShooterGameWithSystems(&window);
    //SDL_Delay(1000);
    //window->PumpEvents();
    //SpaceShooter::SpaceShooterGame(window);

    //ECSGameLoop(window);

    //delete window;

    LOG_INFO("debug", "Textures should be deleted");
    Engine::Close();

    return 0;
}*/
