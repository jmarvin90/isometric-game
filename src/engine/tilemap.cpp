#include <cmath>

#include "SDL2/SDL_image.h"
#include "SDL2/SDL.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "tilemap.h"
#include "constants.h"


// Create the vector of tile entities and load the mousemap surface.
TileMap::TileMap(entt::registry& registry) {
    spdlog::info("TileMap constructor called.");

    // Create the entities associated with the map
    for (int x=0; x<constants::MAP_SIZE_N_TILES; x++) {
        std::vector<entt::entity> row;
        for (int y=0; y<constants::MAP_SIZE_N_TILES; y++) {
            row.push_back(registry.create());
        }
        tilemap.push_back(row);
    }
}

// Question re. destruction of entities
TileMap::~TileMap() {
    spdlog::info("TileMap destructor called.");
}

// Get an entity from tilemap position x, y
entt::entity TileMap::at(const int x, const int y) const {
    return tilemap.at(x).at(y);
}

// Public function converting x, y tilemap coordinates to screen coordinates
glm::ivec2 TileMap::grid_to_pixel(const glm::ivec2& grid_pos) const {
    
    glm::ivec2 offset {
        (grid_pos.x - grid_pos.y) * constants::TILE_SIZE_HALF.x, 
        (grid_pos.y + grid_pos.x) * constants::TILE_SIZE_HALF.y
    };

    return glm::ivec2 {
        // TODO: investigate streamlining this into a glm::vec2 op
        constants::TILEMAP_START + offset
    };
}