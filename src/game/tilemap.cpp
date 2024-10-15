#include <cmath>

#include "SDL2/SDL_image.h"
#include "SDL2/SDL.h"
#include "spdlog/spdlog.h"
#include <glm/glm.hpp>

#include "tilemap.h"
#include "constants.h"


// Create the vector of tile entities and load the mousemap surface.
TileMap::TileMap(entt::registry& registry) {
    spdlog::info("TileMap constructor called.");

    // Create the entities associated with the map
    for (int x=0; x<constants::MAP_SIZE; x++) {
        std::vector<entt::entity> row;
        for (int y=0; y<constants::MAP_SIZE; y++) {
            row.push_back(registry.create());
        }
        tilemap.push_back(row);
    }
}

// Ensure to free the mousemap surface. Question re. destruction of entities
TileMap::~TileMap() {
    spdlog::info("TileMap destructor called.");
}

// Get an entity from tilemap position x, y
entt::entity TileMap::at(const int x, const int y) {
    return tilemap.at(x).at(y);
}

// Public function converting x, y tilemap coordinates to screen coordinates
glm::ivec2 TileMap::grid_to_pixel(const int x, const int y) const {
    int x_offset {x-y};
    int y_offset {y+x};
    return glm::ivec2 {
        constants::TILEMAP_X_START + (x_offset * constants::TILE_WIDTH_HALF),
        constants::TILEMAP_Y_START + (y_offset * constants::TILE_HEIGHT_HALF)
    };
}