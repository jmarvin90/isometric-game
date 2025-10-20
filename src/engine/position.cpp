#include <position.h>

WorldPosition ScreenPosition::to_world_position(const glm::ivec2 camera_position) const {
    return WorldPosition(m_tilemap, m_position + camera_position);
}

ScreenPosition WorldPosition::to_screen_position(const glm::ivec2 camera_position) const {
    return ScreenPosition{m_tilemap, m_position - camera_position};
}

GridPosition WorldPosition::to_grid_position() const {
    const glm::vec2 gross {to_grid_gross()};
    return GridPosition{
        m_tilemap, 
        glm::ivec2(std::round(gross.x), std::round(gross.y))
    };   
}

const glm::vec2 WorldPosition::to_grid_gross() const {
    const glm::ivec2 world_pos_adjusted {m_position - (m_tilemap.tile_spec().size / 2)};
    const glm::ivec2 centred_world_pos {world_pos_adjusted - m_tilemap.origin_px()};
    return glm::vec2 {m_tilemap.tile_spec().matrix_inverted * centred_world_pos};
}

const WorldPosition GridPosition::to_world_position() const {
    const glm::ivec2 movement {m_tilemap.tile_spec().matrix * m_position};

    const glm::ivec2 world_pos {
        (movement + m_tilemap.origin_px()) - 
        (m_tilemap.tile_spec().size / 2)
    };

    return WorldPosition(
        m_tilemap, world_pos + (m_tilemap.tile_spec().size / 2)
    );
}