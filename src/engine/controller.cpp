#include "controller.h"

namespace Engine {
const int Game_Controller::AXIS_DEAD_ZONE = 16000;

CONTROLLER_SCHEME Game_Controller::controller_scheme = CONTROLLER_SCHEME_CONTROLLERS;

Game_Controller::Game_Controller() {
    //Dpad
    up = down = left = right = false;
    //Shoulders
    left_shoulder = right_shoulder = false;
    //Buttons
    A_button = B_button = X_button = Y_button = false;
    //Joystick
    left_stickX = left_stickY = 0;
    right_stickX = right_stickY = 0;
    id = NULL;
    haptic_id = NULL;
    joystick_id = NULL;
}

void Game_Controller::Init(int index) {
    id = SDL_GameControllerOpen(index);
    joystick_id = SDL_JoystickOpen(index);
    haptic_id = SDL_HapticOpenFromJoystick(joystick_id);
    SDL_HapticRumbleInit(haptic_id);
    LOG_INFO("Controller", "Opened game controller: id=[%d]", index);
}

void Game_Controller::Destroy() {
    SDL_GameControllerClose(id);
    SDL_HapticClose(haptic_id);
    LOG_INFO("Controller", "Closed game controller: id=[%d]", id);
    id = NULL;
    haptic_id = NULL;
}

void Game_Controller::Update_Events() {
    if (id != NULL && SDL_GameControllerGetAttached(id)) {
        //Dpad
        up = SDL_GameControllerGetButton(id, SDL_CONTROLLER_BUTTON_DPAD_UP);
        down = SDL_GameControllerGetButton(id, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        left = SDL_GameControllerGetButton(id, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        right = SDL_GameControllerGetButton(id, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        //Shoulders
        left_shoulder = SDL_GameControllerGetButton(id, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
        right_shoulder = SDL_GameControllerGetButton(id, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
        //Buttons
        A_button = SDL_GameControllerGetButton(id, SDL_CONTROLLER_BUTTON_A);
        B_button = SDL_GameControllerGetButton(id, SDL_CONTROLLER_BUTTON_B);
        X_button = SDL_GameControllerGetButton(id, SDL_CONTROLLER_BUTTON_X);
        Y_button = SDL_GameControllerGetButton(id, SDL_CONTROLLER_BUTTON_Y);
        //Joystick
        left_stickX = SDL_GameControllerGetAxis(id, SDL_CONTROLLER_AXIS_LEFTX);
        left_stickY = SDL_GameControllerGetAxis(id, SDL_CONTROLLER_AXIS_LEFTY);
        right_stickX = SDL_GameControllerGetAxis(id, SDL_CONTROLLER_AXIS_RIGHTX);
        right_stickY = SDL_GameControllerGetAxis(id, SDL_CONTROLLER_AXIS_RIGHTY);
        //Misc
        start_button = SDL_GameControllerGetButton(id, SDL_CONTROLLER_BUTTON_START);
        guide_button = SDL_GameControllerGetButton(id, SDL_CONTROLLER_BUTTON_GUIDE);
        back_button = SDL_GameControllerGetButton(id, SDL_CONTROLLER_BUTTON_BACK);
    }

    //printf("{up=%d, down=%d, left=%d, right=%d, A=%d, B=%d, X=%d, Y=%d, Ltr=%d, Rtr=%d, Start=%d}\n",up,down,left,right,A_button,B_button,X_button,Y_button,left_shoulder,right_shoulder,start_button);
}

SDL_GameController *Game_Controller::Get_id() {
    return id;
}

bool Game_Controller::Pressed_Up() {
    return up;
}

bool Game_Controller::Pressed_Down() {
    return down;
}

bool Game_Controller::Pressed_Left() {
    return left;
}

bool Game_Controller::Pressed_Right() {
    return right;
}

bool Game_Controller::Pressed_LeftShoulder() {
    return left_shoulder;
}

bool Game_Controller::Pressed_RightShoulder() {
    return right_shoulder;
}

bool Game_Controller::Pressed_A_button() {
    return A_button;
}

bool Game_Controller::Pressed_B_button() {
    return B_button;
}

bool Game_Controller::Pressed_X_button() {
    return X_button;
}

bool Game_Controller::Pressed_Y_button() {
    return Y_button;
}

bool Game_Controller::Get_Button_State(int index) {
    bool rtn;

    switch (index) {
    case 1:
        rtn = A_button;
        break;

    case 2:
        rtn = B_button;
        break;

    case 3:
        rtn = Y_button;
        break;

    case 4:
        rtn = X_button;
        break;

    default:
        rtn = false;
    }

    return rtn;
}

bool Game_Controller::Pressed_Back_button() {
    return back_button;
}

bool Game_Controller::Pressed_Guide_button() {
    return guide_button;
}

bool Game_Controller::Pressed_Start_button() {
    return start_button;
}

bool Game_Controller::Pressed_Any_Button() {
    bool rtn;
    //Dpad
    rtn = up || down || left || right;
    //Shoulders
    rtn = rtn || left_shoulder || right_shoulder;
    //Buttons
    rtn = rtn || A_button || B_button || X_button || Y_button;
    //Misc
    rtn = rtn || back_button || guide_button || start_button;
    return rtn;
}

int Game_Controller::Get_LeftStickX() {
    return left_stickX;
}

int Game_Controller::Get_LeftStickY() {
    return left_stickY;
}

int Game_Controller::Get_RightStickX() {
    return right_stickX;
}

int Game_Controller::Get_RightStickY() {
    return right_stickY;
}

void Game_Controller::Vibrate(double strength, int duration) {
    SDL_HapticRumblePlay(haptic_id, strength, duration);
}

void Game_Controller::Stop_Vibration() {
    SDL_HapticStopAll(haptic_id);
}
}

