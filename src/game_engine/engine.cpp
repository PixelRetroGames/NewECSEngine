#include "engine.h"

namespace GameEngine {

void GameEngine::Execute(std::string scriptPath, std::string fnName) {
    auto scriptPtr = dynamicScriptManager->Load(scriptPath, fnName);

    if (auto script = scriptPtr.lock()) {
        script->Run();
    }

    dynamicScriptManager->UnLoadLib(scriptPath);
}
};
