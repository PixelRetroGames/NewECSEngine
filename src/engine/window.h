#ifndef WINDOW_H
#define WINDOW_H

#include "utils.h"
#include "surface.h"
#include "renderer.h"
#include "events.h"
#include "controller.h"
#include "window_attributes.h"

namespace Engine {
class Window {
  private:
    Events events;
    SDL_Window *window = NULL;
    SDL_Surface *surface = NULL;
    Renderer renderer;
    Rectangle position = {0, 0, 0, 0};
    std::string title = "", icon = "";
    int flags = 0;
    bool opened = false;

    Game_Controller controller[3];

    void Set_flag(int flag);
    void Reset_flag(int flag);

    void Open_icon();

  public:
    Window() {}
    Window(std::string _title, int w, int h, int _flags = 0, int x = WINDOWPOS_CENTERED, int y = WINDOWPOS_CENTERED, std::string _icon = "");

    void SetArguments(std::string _title, int w, int h, int _flags = 0, int x = WINDOWPOS_CENTERED, int y = WINDOWPOS_CENTERED, std::string _icon = "");
    void SetPosition(int x, int y, int w, int h);
    void SetPosition(Rectangle rect);
    void SetTitle(std::string _title);
    void SetIcon(std::string _icon);
    void SetFlags(int _flags);

    void Open();
    void Open(std::string _title, int w, int h, int _flags = 0, int x = WINDOWPOS_CENTERED, int y = WINDOWPOS_CENTERED, std::string _icon = "");
    void Close();
    void Reopen();
    void Reopen(std::string _title, int w, int h, int _flags = 0, int x = WINDOWPOS_CENTERED, int y = WINDOWPOS_CENTERED, std::string _icon = "");

    bool GetKeystate(const Scancode key);
    const Uint8 *GetKeystates();
    void PumpEvents();

    void Init_Controllers();
    void Destroy_Controllers();
    void Update_Controllers_Events();
    Game_Controller *Get_controller(int id);
    Game_Controller *Get_controllers();

    Renderer *GetRenderer();
    Rectangle GetPosition();
};
}

#endif // WINDOW_H
