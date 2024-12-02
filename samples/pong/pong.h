#pragma once

#include "context.h"

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

extern "C"
{
    DECLDIR DynamicLoader::DynamicScriptInterface *CreateScript(GameEngine::GameContext *ctx);
}


void RunPongGame();
