#pragma once
#include "../src/dynamic_loader/dynamic_script_interface.h"
#include "../src/game_engine/engine.h"

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

extern "C"
{
    DECLDIR DynamicLoader::DynamicScriptInterface *CreateScript(GameEngine::GameContext *ctx);
}
