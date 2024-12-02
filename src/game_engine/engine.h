#pragma once

#include "../engine/window.h"
#include "../ecs/engine.h"

#include "../../src/dynamic_loader/dynamic_loader.h"
#include "../../src/dynamic_loader/wrapper_generators.h"

namespace GameEngine {

class GameEngine;

EVAL(CREATE_CONTEXT(GameContext, (GameEngine, gameEngine)))

class GameEngine {
  private:

    GameContext *gameContext;
    DynamicLoader::DynamicScriptManager<GameContext> *dynamicScriptManager;

  public:
    GameEngine() {
        gameContext = new GameContext(this);
        dynamicScriptManager = new DynamicLoader::DynamicScriptManager<GameContext>(gameContext);
    }

    void Execute(std::string scriptPath, std::string fnName);
};

};
