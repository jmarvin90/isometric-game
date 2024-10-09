#include <SDL2/SDL_image.h>
#include "spdlog/spdlog.h"
#include "mousemap.h"


MouseMap::MouseMap(const std::string mousemap_file_path): 
    mousemap{IMG_Load(mousemap_file_path.c_str())} 
{
    spdlog::info("MouseMap constructor called.");
    if (!mousemap) {
        spdlog::info("Could not load mouse map!");
    }
}

MouseMap::~MouseMap() {
    spdlog::info("MouseMap destuctor called.");
    SDL_FreeSurface(mousemap);
}

SDL_Color MouseMap::get_pixel(const int x, const int y) const {
    const unsigned int bpp {mousemap->format->BytesPerPixel};
    
    // TODO: understand wtf happens here
    // Void pointer converted to int pointer and dereferenced?
    unsigned char* pixel {(Uint8*)mousemap->pixels + y * mousemap->pitch + x * bpp};
    unsigned int pixel_data = *(unsigned int*)pixel;

    SDL_Color pixel_colour {0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE};

    SDL_GetRGB(
        pixel_data,
        mousemap->format,
        &pixel_colour.r,
        &pixel_colour.g,
        &pixel_colour.b
    );

    return pixel_colour;
}