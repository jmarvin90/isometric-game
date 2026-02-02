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

struct WorldPosition {
    glm::ivec2 position;
};
struct SpatialMapGridPosition {
    glm::ivec2 position;
};

glm::vec2 to_grid_gross(const WorldPosition& position, const entt::registry& registry);

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

WorldPosition to_world_position(const TileMapGridPositionComponent& position, const entt::registry& registry);
WorldPosition to_world_position(const ScreenPositionComponent& position, const entt::registry& registry);
WorldPosition to_world_position(const SpatialMapGridPosition& position, const SpatialMapComponent& spatial_map);
WorldPosition to_world_position(const SpatialMapGridPosition& position, const entt::registry& registry);

TileMapGridPositionComponent to_grid_position(const ScreenPositionComponent& position, const entt::registry& registry);
TileMapGridPositionComponent to_grid_position(const WorldPosition& position, const entt::registry& registry);

TileMapGridPositionComponent from_tile_number(const entt::registry& registry, const int cell_number);

ScreenPositionComponent to_screen_position(const WorldPosition& position, const CameraComponent& camera);
ScreenPositionComponent to_screen_position(const WorldPosition& position, const entt::registry& registry);

SpatialMapGridPosition to_spatial_map_grid_position(const TileMapGridPositionComponent& position, const entt::registry& registry);
SpatialMapGridPosition to_spatial_map_grid_position(const TileMapGridPositionComponent& position, const SpatialMapComponent& spatial_map);
SpatialMapGridPosition to_spatial_map_grid_position(const WorldPosition& position, const entt::registry& registry);
SpatialMapGridPosition to_spatial_map_grid_position(const WorldPosition& position, const SpatialMapComponent& spatial_map);

int to_spatial_map_cell(const WorldPosition& position, const SpatialMapComponent& spatial_map);
int to_spatial_map_cell(const WorldPosition& position, const entt::registry& registry);
int to_spatial_map_cell(const SpatialMapGridPosition& position, const SpatialMapComponent& spatial_map);
int to_spatial_map_cell(const SpatialMapGridPosition& position, const entt::registry& registry);

SpatialMapGridPosition from_cell_number(const entt::registry& registry, const int cell_number);
SpatialMapGridPosition from_cell_number(const SpatialMapComponent& spatial_map, const int cell_number);

bool is_valid(const TileMapGridPositionComponent& position, const TileMapComponent& tilemap);
bool is_valid(const TileMapGridPositionComponent& position, const entt::registry& registry);
bool is_valid(const WorldPosition& position, const entt::registry& registry);
bool is_valid(const WorldPosition& position, const TileMapComponent& tilemap);
bool is_valid(const SpatialMapGridPosition& position, const entt::registry& registry);
bool is_valid(const SpatialMapGridPosition& position, const SpatialMapComponent& spatial_map);
bool is_valid(const ScreenPositionComponent& position, const SDL_DisplayMode& display_mode);
}

#endif