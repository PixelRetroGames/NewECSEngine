#pragma once

#include "glm.hpp"
#include <string>
#include <sstream>
#include <iomanip>

#include "biecs/biecs.h"

namespace Pong {

struct PlayerMovement {
    Engine::Scancode keyUp, keyDown;
    PlayerMovement(Engine::Scancode keyUp, Engine::Scancode keyDown) : keyUp(keyUp), keyDown(keyDown) {}
};

struct Wall {};

struct Exit {
    int playerScoreID;
    Exit(int playerScoreID) : playerScoreID(playerScoreID) {}
};

struct PlayerCollision {};

struct WallCollision {};

struct ExitCollision {};

struct ScoreBoard {
    bool dirty = true;
    int scores[2];
};

struct Centered {
    BIECS::Transform2D anchor;

    Centered(BIECS::Transform2D anchor) : anchor(anchor) {}
};

}
