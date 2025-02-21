#include <cmath>
#include <iostream>

#include "SDL2/SDL_image.h"
#include "SDL2/SDL.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "tilemap.h"
#include "constants.h"

#include "components/transform.h"
#include "components/sprite.h"

Tile::Tile(entt::registry& registry, const glm::ivec2 grid_position): 
    registry{registry}, 
    grid_position{grid_position}, 
    entity{registry.create()}
{}

const glm::ivec2 Tile::world_position() const {
    return glm::ivec2 {
        constants::TILEMAP_START + (
            glm::ivec2(
                grid_position.x - grid_position.y, 
                grid_position.y + grid_position.x
            ) * 
            constants::TILE_SIZE_HALF
        )
    };
}

Tile::~Tile() {
    registry.destroy(entity);
}

entt::entity Tile::add_building_level(SDL_Texture* texture, SDL_Rect sprite_rect) {

    // Create the entity and get the 'z-index'
    entt::entity& level {building_levels.emplace_back(registry.create())};
    auto vertical_level {building_levels.size()};

    // Determine the vertical offset based on the building 'level'
    int offset = constants::GROUND_FLOOR_BUILDING_OFFSET + (
        (vertical_level == 1) ? 0 : constants::MAX_TILE_DEPTH * (vertical_level -1)
    );

    // Create the necessary components
    registry.emplace<Transform>(
        level, world_position(), vertical_level, 0.0
    );

    registry.emplace<Sprite>(
        level,
        texture,
        sprite_rect,
        glm::vec2{0, offset}
    );

    return level;
}


// Create the vector of tile entities and load the mousemap surface.
TileMap::TileMap(entt::registry& registry)
    // : tilemap(pow(constants::MAP_SIZE_N_TILES, 2)) 
{
    spdlog::info("TileMap constructor called.");

    // Reserve exactly the right amount of memory for the tilemap
    tilemap.reserve(pow(constants::MAP_SIZE_N_TILES, 2));

    // TODO - can I do this in the constructor initialiser list
    for (int cell=0; cell<pow(constants::MAP_SIZE_N_TILES, 2); cell++) {
        glm::ivec2 grid_position{};

        if (cell < constants::MAP_SIZE_N_TILES) {
            grid_position = {cell, 0};
        } else {
            grid_position = {
                cell % constants::MAP_SIZE_N_TILES,
                cell / constants::MAP_SIZE_N_TILES
            };
        }

        tilemap.emplace_back(
            registry, 
            grid_position
        );
    }
}

// Question re. destruction of entities
TileMap::~TileMap() {
    spdlog::info("TileMap destructor called.");
}

// Get an entity from tilemap position x, y
Tile& TileMap::at(const int x, const int y) {
    return tilemap.at((y * constants::MAP_SIZE_N_TILES) + x);
}

// Public function converting x, y tilemap coordinates to screen coordinates
glm::ivec2 TileMap::grid_to_pixel(glm::ivec2 grid_pos) {
    return at(grid_pos.x, grid_pos.y).world_position();
}