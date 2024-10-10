#include "SDL2/SDL_image.h"
#include "SDL2/SDL.h"
#include "spdlog/spdlog.h"
#include "glm/glm.hpp"

#include "tilemap.h"
#include "constants.h"


TileMap::TileMap(entt::registry& registry) {
    spdlog::info("TileMap constructor called.");
    for (int x=0; x<constants::MAP_SIZE; x++) {
        std::vector<entt::entity> row;
        for (int y=0; y<constants::MAP_SIZE; y++) {
            row.push_back(registry.create());
        }
        tilemap.push_back(row);
    }
}

TileMap::~TileMap() {
    spdlog::info("TileMap destructor called.");
}

entt::entity TileMap::at(const int x, const int y) {
    return tilemap.at(x).at(y);
}

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

glm::vec2 MouseMap::pixel_to_vector(const SDL_Colour& colour) const {
    if (colour.r == 255) {
        return glm::vec2(-1, 0);
    }

    if (colour.g == 255) {
        return glm::vec2(0, -1);
    }

    if (colour.b == 255) {
        return glm::vec2(1, 0);
    }

    if (colour.r == 0 && colour.g == 0 && colour.b == 0) {
        return glm::vec2(0, 1);
    }

    return glm::vec2(0, 0);
}