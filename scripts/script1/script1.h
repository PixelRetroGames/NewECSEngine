#ifndef _DLL_TUTORIAL_H_
#define _DLL_TUTORIAL_H_
#include <iostream>
#include "../../src/dynamic_loader/dynamic_script_interface.h"
#include "../../space_shooter_game/shared.h"

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

extern "C"
{
    DECLDIR DynamicLoader::DynamicScriptInterface *CreateScript1(SpaceShooter::GameContext *ctx);
    DECLDIR DynamicLoader::DynamicScriptInterface *CreateScript2(SpaceShooter::GameContext *ctx);
    DECLDIR DynamicLoader::DynamicScriptInterface *CreatePlayerMovementScript(SpaceShooter::GameContext *ctx);
}

#endif

