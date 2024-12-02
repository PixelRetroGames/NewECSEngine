#include "surface_factory.h"

namespace Engine {

namespace Impl {

Surface NULLSurface = Surface();
Surface *NULLSurfacePtr = &NULLSurface;

const SDL_Color TRANSPARENT_COLOR = {0xFF, 0x0, 0xE1, 0};

SurfaceFactory::SurfaceFactory() {
    NULLSurfacePtr = &NULLSurface;
    Open();
}

void SurfaceFactory::Open() {
    LOG_INFO("Surface", "Surface factory opened");
}

SurfaceFactory::~SurfaceFactory() {
    Close();
}

void SurfaceFactory::Close() {
    LOG_INFO("Surface", "Surface factory closed");
}

static SDL_Surface *LoadSurface(std::string filename) {
    SDL_Surface *loadedImage = NULL;
    loadedImage = IMG_Load(filename.c_str());

    if (loadedImage != NULL) {
        Uint32 colorkey = SDL_MapRGB(loadedImage->format, TRANSPARENT_COLOR.r, TRANSPARENT_COLOR.g, TRANSPARENT_COLOR.b);
        Uint8 r, g, b;
        SDL_GetRGB(colorkey, loadedImage->format, &r, &g, &b);

        if (r == TRANSPARENT_COLOR.r &&
                g == TRANSPARENT_COLOR.g &&
                b == TRANSPARENT_COLOR.b)
            SDL_SetColorKey(loadedImage, SDL_TRUE, colorkey);
    }

    return loadedImage;
}

Surface *SurfaceFactory::Load(std::string filename) const {
    if (filename.empty()) {
        LOG_WARNING("Surface", "Could not load surface: filename is empty");
        return NULLSurfacePtr;
    }

    SDL_Surface *loadedImage = LoadSurface(filename.c_str());

    if (loadedImage == NULL) {
        LOG_WARNING("Surface", "Could not load surface: %s", SDL_GetError());
        return NULLSurfacePtr;
    }

    //SDL_Surface *optimizedImage=SDL_ConvertSurface(loadedImage,format,NULL);
    //SDL_FreeSurface(loadedImage);
    /*if(optimizedImage==NULL)
       {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,"Could not optimize surface",SDL_GetError(),NULL);
        return NULLSurfacePtr;
       }*/

    //Surface *surface;
    //surface=new Surface(optimizedImage);
    Surface *surface = new Surface(loadedImage);

    LOG_INFO("Surface", "Loaded surface: %s", filename.c_str());

    return surface;
}

}
}
