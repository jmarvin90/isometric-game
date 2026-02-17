#ifndef POSITION_H
#define POSITION_H

#include <SDL2/SDL.h>
#include <components/camera_component.h>
#include <components/screen_position_component.h>
#include <components/spatialmap_component.h>
#include <components/tilemap_component.h>
#include <components/tilemap_grid_position_component.h>

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

glm::vec2 to_grid_gross(
    const glm::ivec2 position,
    const TileSpecComponent& tilespec,
    const TileMapComponent& tilemap
);

/*
    operator glm::ivec2() const { return position; }
    operator glm::vec2() const { return position; }
    operator SDL_Point() const { return SDL_Point { position.x, position.y }; }
*/

template <typename T>
bool _in_min_bounds(const T& position)
{
    return position.x >= 0 && position.y >= 0;
}

namespace Position {

glm::ivec2 to_world_position(
    const TileMapGridPositionComponent& position,
    const TileSpecComponent& tilespec,
    const TileMapComponent& tilemap
);
glm::ivec2 to_world_position(const ScreenPositionComponent& position, const CameraComponent& camera);
glm::ivec2 to_world_position(const SpatialMapGridPosition& position, const SpatialMapComponent& spatial_map);

TileMapGridPositionComponent to_grid_position(
    const glm::ivec2 world_position,
    const TileSpecComponent& tilespec,
    const TileMapComponent& tilemap
);

ScreenPositionComponent to_screen_position(const glm::ivec2 world_position, const CameraComponent& camera);

bool is_valid(const TileMapGridPositionComponent& position, const TileMapComponent& tilemap);
bool is_valid(const glm::ivec2 world_position, const TileMapComponent& tilemap);
bool is_valid(const ScreenPositionComponent& position, const SDL_DisplayMode& display_mode);

}

#endif