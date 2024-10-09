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