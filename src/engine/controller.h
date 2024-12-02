#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "utils.h"
#include "../logging/logging.h"

namespace Engine {
enum CONTROLLER_SCHEME {
    CONTROLLER_SCHEME_CONTROLLERS = 0,
    CONTROLLER_SCHEME_KEYBOARD = 1
};

class Game_Controller {
  public:
    static const int AXIS_DEAD_ZONE;
    static CONTROLLER_SCHEME controller_scheme;


  private:
    SDL_GameController *id;
    SDL_Haptic *haptic_id;
    SDL_Joystick *joystick_id;
    //Dpad
    bool up, down, left, right;
    //Shoulders
    bool left_shoulder, right_shoulder;
    //Buttons
    bool A_button, B_button, X_button, Y_button;
    //Joystick
    int left_stickX, left_stickY;
    int right_stickX, right_stickY;
    //Misc
    bool back_button, guide_button, start_button;



  public:
    Game_Controller();
    void Init(int _id);
    void Destroy();
    void Update_Events();
    SDL_GameController *Get_id();
    bool Pressed_Up();
    bool Pressed_Down();
    bool Pressed_Left();
    bool Pressed_Right();
    bool Pressed_LeftShoulder();
    bool Pressed_RightShoulder();
    bool Pressed_A_button();
    bool Pressed_B_button();
    bool Pressed_X_button();
    bool Pressed_Y_button();
    bool Get_Button_State(int index);
    bool Pressed_Back_button();
    bool Pressed_Guide_button();
    bool Pressed_Start_button();
    bool Pressed_Any_Button();
    int Get_LeftStickX();
    int Get_LeftStickY();
    int Get_RightStickX();
    int Get_RightStickY();
    void Vibrate(double strength, int duration);
    void Stop_Vibration();
};
}

#endif // CONTROLLER_H

