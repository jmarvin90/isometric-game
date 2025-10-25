#ifndef POSITION_H
#define POSITION_H

#include <tilemap.h>
#include <glm/glm.hpp>

class Position {
    protected:
        const TileMap& m_tilemap;
        glm::ivec2 m_position;

    public:
        Position(const TileMap& tilemap)
        : m_tilemap {tilemap}
        , m_position {}
        {}

        Position(const TileMap& tilemap, const glm::ivec2 position)
        : m_tilemap {tilemap}
        , m_position {position} 
        {}

        ~Position() = default;
        Position(const Position&) = delete;
        const glm::ivec2 position() const { return m_position; }
        operator glm::ivec2() const { return m_position; }
};


class WorldPosition;


class GridPosition: public Position {
    public:
        using Position::Position;
        GridPosition(const TileMap& tilemap, const int tile_n);
        const WorldPosition to_world_position() const;
};


class ScreenPosition: public Position {
    public:
        using Position::Position;

        WorldPosition to_world_position(
            const glm::ivec2 camera_position,
            const int scene_border_px
        ) const;
};


class WorldPosition: public Position {    
    const glm::vec2 to_grid_gross() const;

    public:
        using Position::Position;

        ScreenPosition to_screen_position(
            const glm::ivec2 camera_position,
            const int scene_border_px
        ) const;

        GridPosition to_grid_position() const;
};

#endif