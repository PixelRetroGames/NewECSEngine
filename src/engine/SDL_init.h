#pragma once

#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include <cstring>

#include "SDL_includes.h"
#include "../logging/logging.h"

namespace Engine {

enum Status_flags {
    SDL_INITIALIZED     = 1 << 0,
    TTF_INITIALIZED     = 1 << 1,
    MIX_INITIALIZED     = 1 << 2
};

void Init();
void InitSDL();
void InitTTF();
void InitMix();

void Close();
void CloseSDL();
void CloseTTF();
void CloseMix();

void PrintStatus(FILE *out = stderr);

}
