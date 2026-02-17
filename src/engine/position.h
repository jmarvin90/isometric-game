#ifndef POSITION_H
#define POSITION_H

#include <SDL2/SDL.h>
#include <components/camera_component.h>
#include <components/spatialmap_component.h>
#include <components/tilemap_component.h>

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace Position {

glm::ivec2 to_world_position(const glm::ivec2 grid_position, const TileSpecComponent& tilespec, const TileMapComponent& tilemap);
glm::ivec2 to_world_position(const glm::ivec2 screen_position, const CameraComponent& camera);
glm::ivec2 to_world_position(const glm::ivec2 spatial_map_cell_position, const SpatialMapComponent& spatial_map);
glm::ivec2 to_grid_position(const glm::ivec2 world_position, const TileSpecComponent& tilespec, const TileMapComponent& tilemap);
glm::ivec2 to_screen_position(const glm::ivec2 world_position, const CameraComponent& camera);

}

#endif