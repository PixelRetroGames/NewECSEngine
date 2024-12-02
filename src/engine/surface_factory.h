#pragma once

#include "surface.h"

namespace Engine {

namespace Impl {

extern Surface NULLSurface;
extern Surface *NULLSurfacePtr;

class SurfaceFactory {
  private:
    void Open();
    void Close();

  public:
    Surface *Load(std::string filename) const;
    SurfaceFactory();
    ~SurfaceFactory();
};
}
}
