#include <position.h>

WorldPosition ScreenPosition::to_world_position(
    const glm::ivec2 camera_position,
    const int scene_border_px
) const {
    const glm::ivec2 scene_border {scene_border_px, scene_border_px};
    return WorldPosition(m_tilemap, (m_position + camera_position) - scene_border);
}

ScreenPosition WorldPosition::to_screen_position(
    const glm::ivec2 camera_position,
    const int scene_border_px
) const {
    const glm::ivec2 scene_border {scene_border_px, scene_border_px}
;    return ScreenPosition{m_tilemap, (m_position - camera_position) + scene_border};
}

const glm::vec2 WorldPosition::to_grid_gross() const {
    const glm::ivec2 world_pos_adjusted {m_position - (m_tilemap.tile_spec().centre())};
    const glm::ivec2 centred_world_pos {world_pos_adjusted - m_tilemap.origin_px()};
    return glm::vec2 {m_tilemap.tile_spec().matrix_inverted * centred_world_pos};
}

GridPosition WorldPosition::to_grid_position() const {
    const glm::vec2 gross {to_grid_gross()};
    return GridPosition{
        m_tilemap, 
        glm::ivec2(std::round(gross.x), std::round(gross.y))
    };   
}

const WorldPosition GridPosition::to_world_position() const {
    const glm::ivec2 movement {m_tilemap.tile_spec().matrix * m_position};

    const glm::ivec2 world_pos {
        (movement + m_tilemap.origin_px()) - 
        (m_tilemap.tile_spec().centre())
    };

    return WorldPosition(
        m_tilemap, world_pos + (m_tilemap.tile_spec().centre())
    );
}