#ifndef EVENTS_H
#define EVENTS_H

#include "SDL.h"
#include "scancodes.h"

namespace Engine {
class Events {
  public:
    const Uint8 *keystates;
    Events() : keystates(SDL_GetKeyboardState(NULL)) {}
    void PumpEvents();
};
}

#endif // EVENTS_H
