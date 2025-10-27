#ifndef POSITIONSYSTEM_H
#define POSITIONSYSTEM_H

#include <glm/glm.hpp>
#include <entt/entt.hpp>

class Position {
    protected:
        glm::ivec2 position;

    public:
        Position() = default;
        Position(const glm::ivec2 position): position {position} {} 
        operator glm::ivec2() const { return position; }
};

class WorldPosition;

class ScreenPosition: public Position {
    using Position::Position;
    public:
        const WorldPosition to_world_position(entt::registry& registry) const;
};

class GridPosition: public Position {
    using Position::Position;
    public:
        GridPosition(entt::registry& registry, const int tile_n);
        const WorldPosition to_world_position(entt::registry& registry) const;

};

class WorldPosition: public Position {
    using Position::Position;
    const glm::vec2 to_grid_gross(entt::registry& registry) const;
    public:
        const ScreenPosition to_screen_position(entt::registry& registry) const;
        const GridPosition to_grid_position(entt::registry& registry) const;
};

#endif