#include "events.h"

namespace Engine {
void Events::PumpEvents() {
    SDL_PumpEvents();
}
}
