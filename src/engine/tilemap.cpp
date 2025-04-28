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
#include "geometry.h"

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

Tile* const Tile::scan(const Direction direction) {

    if (!(direction & m_tile_connection_bitmask)) {
        return this;
    }

    Tile* current_tile {this};
    bool valid {true};

    while (valid) {
        glm::ivec2 next_point {
            current_tile->grid_position + 
            constants::VECTORS.at(direction.direction_index())
        };

        Tile* const next_tile {&(*tilemap)[next_point]};

        if (
            !tilemap->in_bounds(next_point) or
            !(-direction & next_tile->m_tile_connection_bitmask)
        ) {
            return current_tile;
        }

        current_tile = next_tile;

        if (__builtin_popcount(current_tile->m_tile_connection_bitmask) > 2) {
            valid = false;
        }
    }

    return current_tile;
}

void Tile::set_connection_bitmask(const uint8_t connection_bitmask) {
    using namespace constants;

    uint8_t tile_disconnection_bitmask {
        uint8_t(~connection_bitmask & m_tile_connection_bitmask)
    };

    std::array<Tile*, 4> connections{};
    std::array<Tile*, 4> disconnections{};

    for (uint8_t i=Directions::NORTH; i; i>>=1) {
        Direction direction{i};
        
        if (direction & tile_disconnection_bitmask) {
            disconnections.at(direction.direction_index()) = scan(direction);
        }

        m_tile_connection_bitmask &= ~direction;
        m_tile_connection_bitmask |= (direction & connection_bitmask);


        if (direction & m_tile_connection_bitmask) {
            connections.at(direction.direction_index()) = scan(direction);
        }
    }

    for (uint8_t i=Directions::NORTH; i; i>>=1) {

        Direction direction {i};

        Tile* node {disconnections.at(direction.direction_index())};

        if (node) {
            Tile* new_target {node->scan(-direction)};
            if (new_target != node) {
                spdlog::info("Connecting via disconnection");
                tilemap->connect(node, new_target, -direction);
                tilemap->connect(new_target, node, direction);
            }
        }
    }

    if (
        (m_tile_connection_bitmask == (Directions::NORTH | Directions::SOUTH)) |
        (m_tile_connection_bitmask == (Directions::EAST | Directions::WEST))
    ) {
        Direction direction {
            Directions::NORTH & m_tile_connection_bitmask ? 
            Directions::NORTH: Directions::EAST
        };

        Tile* start_node {connections.at(direction.direction_index())};
        Tile* end_node {connections.at((-direction).direction_index())};

        if ((start_node && end_node) && (start_node != end_node)) {
            spdlog::info("Connecting in a straight line");
            tilemap->connect(start_node, end_node, -direction);
            tilemap->connect(end_node, start_node, direction);
        }

    } else {
        for (uint8_t i=8; i; i>>=1) {
            Direction direction{i};
            Tile* node {connections.at(direction.direction_index())};
            if (node && (node != this)) {
                spdlog::info("Connecting via a change in direction");
                tilemap->connect(this, node, direction);
                tilemap->connect(node, this, -direction);
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

bool TileMap::in_bounds(const glm::ivec2 position) const {
    bool x_in_bounds {
        0 <= position.x && size_t(position.x) <= constants::MAP_SIZE_N_TILES
    };

    bool y_in_bounds {
        0 <= position.y && size_t(position.y) <= constants::MAP_SIZE_N_TILES
    };

    return x_in_bounds && y_in_bounds;
}

void TileMap::disconnect(Tile* tile, const Direction direction) {
    bool key_in_graph {graph.find(tile) != graph.end()};

    if (!key_in_graph || !graph.at(tile).at(direction.direction_index())) {
        return;
    }

    graph.at(tile).at(direction.direction_index()) = nullptr;
}

void TileMap::connect(
    Tile* origin, 
    Tile* termination, 
    const Direction direction
) {

    if (!(graph.find(origin) != graph.end())) {
        graph.insert({origin, std::array<Tile*, 4>{}});
    }

    for (auto it = graph.begin(); it != graph.end(); it++) {
        if (it->second.at(direction.direction_index()) == termination) {
            disconnect(it->first, direction);
        }
    }

    graph.at(origin).at(direction.direction_index()) = termination;
}