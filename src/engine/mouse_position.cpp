#include <mouse_position.h>
#include <SDL2/SDL.h>

void MousePosition::update() {
    m_mouse_window_previous_position = m_mouse_window_current_position;

    mouse_state = SDL_GetMouseState(
        &m_mouse_window_current_position.x,
        &m_mouse_window_current_position.y
    );
}

bool MousePosition::moved() const {
    return !(m_mouse_window_current_position == m_mouse_window_previous_position);
}

const GridPosition MousePosition::on_grid() const {
    return WorldPosition(
        tilemap, m_mouse_window_current_position + camera_position
    ).to_grid_position();
}

const ScreenPosition MousePosition::on_screen() const {
    return ScreenPosition(tilemap, m_mouse_window_current_position);
}

const WorldPosition MousePosition::in_world() const {
    return WorldPosition(
        tilemap, m_mouse_window_current_position + camera_position
    );
}

// const glm::vec2 MousePosition::on_grid_gross() const {
//     const glm::ivec2 world_pos_adjusted {in_world() - (tilemap.tile_spec().size / 2)};
//     const glm::ivec2 centred_world_pos {world_pos_adjusted - tilemap.origin_px()};
//     return glm::vec2 {tilemap.tile_spec().matrix_inverted * centred_world_pos};
// }

// const glm::ivec2 MousePosition::on_screen() const {
//     return m_mouse_window_current_position;
// }

// const glm::ivec2 MousePosition::in_world() const {
//     return m_mouse_window_current_position + camera_position;
// }

// const glm::ivec2 MousePosition::on_grid() const {
//     const glm::vec2 gross {on_grid_gross()};
//     return glm::ivec2(std::round(gross.x), std::round(gross.y));
// }