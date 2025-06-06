#include <cmath>
#include <iostream>
#include <queue>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <unordered_map>
#include <utility>

#include "SDL2/SDL_image.h"
#include "SDL2/SDL.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "tilemap.h"
#include "constants.h"
#include "spritesheet.h"

#include "components/transform.h"
#include "components/sprite.h"
#include "utils.h"

Tile::Tile(entt::registry& registry, const glm::ivec2 grid_position, TileMap* tilemap)
    : registry{ registry }
    , grid_position{ grid_position }
    , tilemap{ tilemap }
    , base_entity{ registry.create() }
    , overlay_entity { registry.create() }
{
    // TODO: check if it's strictly necessary for every tile to have an overlay entity/transform
    registry.emplace<Transform>(base_entity, world_position(), 0, 0.0);
    registry.emplace<Transform>(overlay_entity, world_position(), 1, 0.0);
}

glm::ivec2 Tile::world_position() const
{
    return glm::ivec2{
        constants::TILEMAP_START + (
            glm::ivec2(
                grid_position.x - grid_position.y,
                grid_position.y + grid_position.x
            ) * constants::TILE_SIZE_HALF
        )
    };
}

glm::ivec2 Tile::get_centre() const {
    return { world_position() + constants::TILE_SIZE_HALF };
}

Tile::~Tile()
{
    registry.destroy(base_entity);
    registry.destroy(overlay_entity);
}

entt::entity Tile::add_building_level(const Sprite* sprite)
{

    /*
        TODO: there's potentially some logic to be written here for
        e.g. ensuring that a new level isn't created above a roof
    */

    // Create the entity and get the 'z-index'

    /*
        Either
            - there are no building levels at this tile location; in which
              case, we set our starting position using the ground floor offset; 
              or
            - there are levels at this tile location, in which case, we set our
              offset relative to the 'highest' building in building_levels

        For the latter, we
            - start with the offset of the last building level,
            - move up by the full height of the new level sprite image
            - find the height of the new sprite's TILE be halfing the image's 
              width
            - adjust the position down by the difference between the sprite's
              total height and tile height

        The logic is in the fact that all tile-like sprites will contain
        an isometric diamond whose height is half its width (2:1). In addition
        to the diamond, they may contain a vertical component to represent
        e.g. the height of a building floor. We can find the height portion
        by subtracting the total image height from the isometric tile height.
    */
    glm::ivec2 offset {0};

    if (building_levels.empty()) {
        offset.y -= constants::GROUND_FLOOR_BUILDING_OFFSET;
    } else {
        offset.y += registry.get<Sprite>(building_levels.back()).offset.y;
        offset.y -= (sprite->source_rect.h - (sprite->source_rect.w / 2));
        offset.y += 1;      // TODO: where does this 1 come from?
    }

    // Centre against the tile - building levels aren't full tile widths
    offset.x += (
        ((constants::TILE_SIZE.x - sprite->source_rect.w) / 2) + 
        ((constants::TILE_SIZE.x - sprite->source_rect.w) % 2 != 0)
    );
        

    entt::entity& level{ building_levels.emplace_back(registry.create()) };
    auto vertical_level{ building_levels.size() };

    // Create the necessary components
    registry.emplace<Transform>(level, world_position(), vertical_level, 0.0);
    registry.emplace<Sprite>(level, sprite->texture, sprite->source_rect, offset);

    return level;
}

void Tile::set_tile_base(const Sprite* target_sprite) {

    glm::ivec2 offset {0, constants::TILE_BASE_HEIGHT - target_sprite->source_rect.h};

    spdlog::info("Setting base sprite");
    std::remove_const_t<Sprite>* base_sprite = &registry.emplace_or_replace<Sprite>(
        base_entity, 
        *target_sprite
        // target_sprite->texture,
        // target_sprite->source_rect,
        // glm::ivec2{0, constants::TILE_BASE_HEIGHT - target_sprite->source_rect.h}
    );

    base_sprite->offset = offset;
    set_connection_bitmask(target_sprite->connection);

    spdlog::info("Setting overlay sprite");
    registry.remove<Sprite>(overlay_entity);

    spdlog::info("Setting conditional");
    if (target_sprite->source_rect.h > constants::STANDARD_BASE_TILE_HEIGHT) {
        std::remove_const_t<Sprite>* overlay_sprite {
            &registry.emplace<Sprite>(overlay_entity, *target_sprite)
        };

        overlay_sprite->offset = base_sprite->offset;
        overlay_sprite->source_rect.h -= (
            constants::TILE_BASE_HEIGHT + constants::TILE_SIZE_HALF.y
        );
    }
}


const Tile* TileMap::scan(const glm::ivec2 from, const uint8_t direction) const
{

    const Tile* from_tile{ &(*this)[from] };
    if (!(direction & from_tile->m_tile_connection_bitmask))
    {
        return from_tile;
    }

    const Tile* current_tile{ from_tile };
    bool valid{ true };

    while (valid)
    {
        glm::ivec2 next_point{
            current_tile->grid_position +
            constants::VECTORS.at(direction_index(direction)) };

        const Tile* next_tile{ &(*this)[next_point] };

        if (
            !in_bounds(next_point) or
            !(reverse_direction(direction) & next_tile->m_tile_connection_bitmask))
        {
            return current_tile;
        }

        current_tile = next_tile;

        if (__builtin_popcount(current_tile->m_tile_connection_bitmask & 15) > 2)
        {
            valid = false;
        }
    }

    return current_tile;
}

void Tile::set_connection_bitmask(const uint8_t connection_bitmask)
{
    using namespace constants;

    uint8_t tile_disconnection_bitmask{
        uint8_t(~connection_bitmask & m_tile_connection_bitmask) };

    std::array<const Tile*, 4> connections{};
    std::array<const Tile*, 4> disconnections{};

    for (uint8_t direction = Directions::NORTH; direction; direction >>= 1)
    {

        if (direction & tile_disconnection_bitmask)
        {
            disconnections.at(direction_index(direction)) = tilemap->scan(grid_position, direction);
        }

        m_tile_connection_bitmask &= ~direction;
        m_tile_connection_bitmask |= (direction & connection_bitmask);

        if (direction & m_tile_connection_bitmask)
        {
            connections.at(direction_index(direction)) = tilemap->scan(grid_position, direction);
        }
    }

    for (uint8_t direction = Directions::NORTH; direction; direction >>= 1)
    {
        const Tile* node{ disconnections.at(direction_index(direction)) };

        if (node)
        {
            const Tile* new_target{ tilemap->scan(node->grid_position, reverse_direction(direction)) };
            if (new_target != node)
            {
                spdlog::info("Connecting via disconnection");
                tilemap->connect(node, new_target, reverse_direction(direction));
                tilemap->connect(new_target, node, direction);
            }
        }
    }

    if (
        (m_tile_connection_bitmask == (Directions::NORTH | Directions::SOUTH)) |
        (m_tile_connection_bitmask == (Directions::EAST | Directions::WEST)))
    {
        uint8_t direction{
            Directions::NORTH & m_tile_connection_bitmask ? Directions::NORTH : Directions::EAST };

        uint8_t opposite_direction{ reverse_direction(direction) };

        const Tile* start_node{ connections.at(direction_index(direction)) };
        const Tile* end_node{ connections.at(direction_index(opposite_direction)) };

        if ((start_node && end_node) && (start_node != end_node))
        {
            tilemap->connect(start_node, end_node, opposite_direction);
            tilemap->connect(end_node, start_node, direction);
        }
    }
    else
    {
        for (uint8_t direction = Directions::NORTH; direction; direction >>= 1)
        {
            const Tile* node{ connections.at(direction_index(direction)) };
            if (node && (node != this))
            {
                tilemap->connect(this, node, direction);
                tilemap->connect(node, this, reverse_direction(direction));
            }
        }
    }
}

// Create the vector of tile entities and load the mousemap surface.
TileMap::TileMap(entt::registry& registry)
{
    spdlog::info("TileMap constructor called.");

    // Reserve exactly the right amount of memory for the tilemap
    tilemap.reserve(pow(constants::MAP_SIZE_N_TILES, 2));

    // Emplace entities into the vector
    for (int cell = 0; cell < pow(constants::MAP_SIZE_N_TILES, 2); cell++)
    {
        glm::ivec2 grid_position{};

        if (cell < constants::MAP_SIZE_N_TILES)
        {
            grid_position = { cell, 0 };
        }
        else
        {
            grid_position = {
                cell % constants::MAP_SIZE_N_TILES,
                cell / constants::MAP_SIZE_N_TILES };
        }

        tilemap.emplace_back(
            registry,
            grid_position,
            this);
    }
}

// Question re. destruction of entities
TileMap::~TileMap()
{
    spdlog::info("TileMap destructor called.");
}

// Get an entity from tilemap position x, y
Tile& TileMap::operator[](const glm::ivec2 position)
{
    return tilemap.at(
        (position.y * constants::MAP_SIZE_N_TILES) + position.x);
}

// Get an entity from tilemap position x, y
const Tile& TileMap::operator[](const glm::ivec2 position) const
{
    return tilemap.at(
        (position.y * constants::MAP_SIZE_N_TILES) + position.x);
}

// Public function converting x, y tilemap coordinates to screen coordinates
glm::ivec2 TileMap::grid_to_pixel(glm::ivec2 grid_pos)
{
    return (*this)[{grid_pos.x, grid_pos.y}].world_position();
}

// Fill up an array with world-adjusted points used to highlight a tile
void Tile::get_tile_iso_points(
    SDL_Point* point_array, const glm::ivec2& camera_position) const
{
    glm::ivec2 start_point{ world_position() -= camera_position };
    // start_point.y -= constants::MIN_TILE_DEPTH;

    for (int i = 0; i < 5; i++)
    {
        glm::ivec2 point{
            (constants::TILE_EDGE_POINTS[i] + start_point)
            //  + glm::ivec2{0, constants::MIN_TILE_DEPTH}
        };
        point_array[i] = SDL_Point{ point.x, point.y };
    }
}

void TileMap::disconnect(const Tile* tile, const uint8_t direction)
{
    bool key_in_graph{ graph.find(tile) != graph.end() };

    if (!key_in_graph || !graph.at(tile).at(direction_index(direction)))
    {
        return;
    }

    graph.at(tile).at(direction_index(direction)) = nullptr;
}

void TileMap::connect(
    const Tile* origin,
    const Tile* termination,
    const uint8_t direction)
{

    if (!(graph.find(origin) != graph.end()))
    {
        graph.insert({ origin, std::array<const Tile*, 4>{} });
    }

    for (auto it = graph.begin(); it != graph.end(); it++)
    {
        if (it->second.at(direction_index(direction)) == termination)
        {
            disconnect(it->first, direction);
        }
    }

    graph.at(origin).at(direction_index(direction)) = termination;
}

void TileMap::get_path_between(
    glm::ivec2 point_a, glm::ivec2 point_b, std::vector<glm::ivec2>& path)
{
    typedef std::pair<int, glm::ivec2> priority_point;

    static constexpr auto cmp = [](const priority_point& a, const priority_point& b)
        {
            return a.first < b.first;
        };

    std::priority_queue<priority_point, std::vector<priority_point>, decltype(cmp)> frontier(cmp);
    frontier.push({ 0, point_a });

    std::unordered_map<glm::ivec2, const glm::ivec2*> came_from{ {point_a, nullptr} };
    std::unordered_map<glm::ivec2, int> cost_so_far{ {point_a, 0} };

    while (!frontier.empty())
    {
        const glm::ivec2 current{ frontier.top().second };
        frontier.pop();

        if (current == point_b)
        {
            break;
        }

        // TODO: learn about iterators
        for (auto node_connection : graph)
        {
            for (uint8_t direction = constants::Directions::NORTH; direction; direction >>= 2)
            {
                const Tile* connection{ node_connection.second[direction_index(direction)] };
                if (connection)
                {
                    int new_cost{
                        cost_so_far.at(current) +
                        distance_between(current, connection->grid_position) };

                    if (
                        cost_so_far.find(connection->grid_position) == cost_so_far.end() or
                        new_cost < cost_so_far.at(connection->grid_position))
                    {
                        cost_so_far.emplace(connection->grid_position, new_cost);
                        frontier.emplace(new_cost, connection->grid_position);
                        came_from.emplace(connection->grid_position, &current);
                    }
                }
            }
        }
    }

    glm::ivec2 current{ point_b };
    while (current != point_a)
    {
        path.emplace(path.begin(), current);
        current = *came_from.at(current);
    }
    path.emplace(path.begin(), point_a);
}