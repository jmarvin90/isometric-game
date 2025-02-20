#include <cmath>

#include "SDL2/SDL_image.h"
#include "SDL2/SDL.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "tilemap.h"
#include "constants.h"


// Create the vector of tile entities and load the mousemap surface.
TileMap::TileMap(entt::registry& registry)
    // : tilemap(pow(constants::MAP_SIZE_N_TILES, 2)) 
{
    spdlog::info("TileMap constructor called.");

    // Reserve exactly the right amount of memory for the tilemap
    tilemap.reserve(pow(constants::MAP_SIZE_N_TILES, 2));

    // TODO - can I do this in the constructor initialiser list
    for (int cell=0; cell<pow(constants::MAP_SIZE_N_TILES, 2); cell++) {
        tilemap.emplace_back(registry.create());
    }
}

// Question re. destruction of entities
TileMap::~TileMap() {
    spdlog::info("TileMap destructor called.");
}

// Get an entity from tilemap position x, y
entt::entity TileMap::at(const int x, const int y) const {
    return tilemap.at((y * constants::MAP_SIZE_N_TILES) + x);
}

// Public function converting x, y tilemap coordinates to screen coordinates
glm::ivec2 TileMap::grid_to_pixel(const glm::ivec2& grid_pos) const {
    return glm::ivec2 {
        constants::TILEMAP_START + (
            glm::ivec2(grid_pos.x - grid_pos.y, grid_pos.y + grid_pos.x) * 
            constants::TILE_SIZE_HALF
        )
    };
}