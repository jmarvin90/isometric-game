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

Tile::Tile(entt::registry& registry, const glm::ivec2 grid_position, TileMap* tilemap): 
    registry{registry}, 
    grid_position{grid_position},
    tilemap{tilemap},
    entity{registry.create()}
{}

glm::ivec2 Tile::world_position() const {
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

entt::entity Tile::add_building_level(SDL_Texture* texture, const SDL_Rect sprite_rect) {

    /*
        TODO: there's potentially some logic to be written here for 
        e.g. ensuring that a new level isn't created above a roof
    */

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

void Tile::connect(const uint8_t direction, Tile* tile) {
    glm::ivec2 this_position {get_grid_position()};
    glm::ivec2 that_position {tile->get_grid_position()};
    spdlog::info(
        "Connecting tile at position " + 
        std::to_string(this_position.x) + "," + 
        std::to_string(this_position.y) + 
        " to tile at position " +
        std::to_string(that_position.x) + "," + 
        std::to_string(that_position.y) + " (" +
        std::to_string(direction) + ")"
    );

    uint8_t opposite_direction {
        uint8_t((direction >> 2 | direction << 2) & 15)
    };
    connections[direction] = tile;
    tile->connections[opposite_direction] = this;
}

Tile* Tile::scan(const uint8_t direction) {
    uint8_t opposite_direction {
        uint8_t((direction >> 2 | direction << 2) & 15)
    };

    bool valid {true};
    Tile* current_tile {this};

    while (valid) {
        // TODO: this doesn't account for the edge of the tilemap!!
        Tile* next_tile = &(*tilemap)[
            current_tile->get_grid_position() + constants::VECTORS.at(direction)
        ];

        Tile* connected_tile {next_tile->connections[direction]};

        if (connected_tile) {
            connected_tile->connections[opposite_direction] = nullptr;
            connected_tile = nullptr;
            return next_tile;
        }

        valid = next_tile->tile_connection_bitmask & opposite_direction;

        if (valid) {
            current_tile = next_tile;
        }
    }
    return current_tile;
}

void Tile::set_connection_bitmask(const uint8_t connection_bitmask) {
    // TODO: think about how to streamline this section; it's quite unwieldy
    tile_connection_bitmask = connection_bitmask;

    // If the tile is connected in a straight line...
    if (
        connection_bitmask == (constants::Directions::EAST | constants::Directions::WEST) ||
        connection_bitmask == (constants::Directions::NORTH | constants::Directions::SOUTH)
    ) {
        uint8_t incoming {
            connection_bitmask & constants::Directions::NORTH ? constants::Directions::NORTH : constants::Directions::EAST
        };

        uint8_t outgoing {uint8_t(incoming >> 2)};

        Tile* connection_from_incoming {scan(incoming)};
        Tile* connection_to_outgoing {scan(outgoing)};

        // ... And that straight line doesn't start AND end in the same place (here)...
        if (connection_from_incoming != connection_to_outgoing) {
            /// ... connect the start and end point of that line together
            connection_from_incoming->connect(incoming, connection_to_outgoing);
            return;
        }
    } 
    // otherwise, we must change direction somehow
    else {
        // so connect any remote tiles to this one
        for (int direction=constants::Directions::SOUTH; direction<=constants::Directions::NORTH; direction*=2) {
            if (connection_bitmask & direction) {
                Tile* connection_to_outgoing {scan(direction)};
                if (connection_to_outgoing != this) {
                    connect(direction, connection_to_outgoing);
                }
            }
        }
    }
}


// Create the vector of tile entities and load the mousemap surface.
TileMap::TileMap(entt::registry& registry) {
    spdlog::info("TileMap constructor called.");

    // Reserve exactly the right amount of memory for the tilemap
    tilemap.reserve(pow(constants::MAP_SIZE_N_TILES, 2));

    // Emplace entities into the vector
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
            grid_position,
            this
        );
    }
}

// Question re. destruction of entities
TileMap::~TileMap() {
    spdlog::info("TileMap destructor called.");
}

// Get an entity from tilemap position x, y
Tile& TileMap::operator[](const glm::ivec2 position) {
    return tilemap.at(
        (position.y * constants::MAP_SIZE_N_TILES) + position.x
    );
}

// Public function converting x, y tilemap coordinates to screen coordinates
glm::ivec2 TileMap::grid_to_pixel(glm::ivec2 grid_pos) {
    return (*this)[{grid_pos.x, grid_pos.y}].world_position();
}

// Fill up an array with world-adjusted points used to highlight a tile
void Tile::get_tile_iso_points(
    SDL_Point* point_array, const glm::ivec2& camera_position
) const {
    glm::ivec2 start_point {world_position() -= camera_position};
    start_point.y -= constants::MIN_TILE_DEPTH;
    
    for (int i=0; i<5; i++) {
        glm::ivec2 point {
            (constants::TILE_EDGE_POINTS[i] + start_point)
            //  + glm::ivec2{0, constants::MIN_TILE_DEPTH} 
        };
        point_array[i] = SDL_Point{point.x, point.y}; 
    }
}