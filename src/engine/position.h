#ifndef POSITION_H
#define POSITION_H

#include <SDL2/SDL.h>
#include <components/tilemap_component.h>
#include <components/camera_component.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

class IPosition {
protected:
    bool _in_min_bounds() const;
    glm::ivec2 position;

public:
    IPosition() = default;
    IPosition(const glm::ivec2 position)
        : position { position }
    {
    }
    operator glm::ivec2() const { return position; }
};

class WorldPosition;
class GridPosition;

class ScreenPosition : public IPosition {
    using IPosition::IPosition;

public:
    const WorldPosition to_world_position(const entt::registry& registry) const;
    const GridPosition to_grid_position(const entt::registry& registry) const;
    bool is_valid(const SDL_DisplayMode& display_mode) const;
};

class TileMapComponent;

class GridPosition : public IPosition {
    using IPosition::IPosition;

public:
    GridPosition(const entt::registry& registry, const int tile_n);
    const WorldPosition to_world_position(const entt::registry& registry) const;
    bool is_valid(const TileMapComponent& tilemap) const;
    bool is_valid(const entt::registry& registry) const;
};

class WorldPosition : public IPosition {
    using IPosition::IPosition;
    const glm::vec2 to_grid_gross(const entt::registry& registry) const;

public:
    WorldPosition(const entt::registry& registry, const entt::entity entity);
    const ScreenPosition to_screen_position(const CameraComponent& camera) const;
    const ScreenPosition to_screen_position(const entt::registry& registry) const;
    const GridPosition to_grid_position(const entt::registry& registry) const;
    bool is_valid(const entt::registry& registry) const;
};

#endif