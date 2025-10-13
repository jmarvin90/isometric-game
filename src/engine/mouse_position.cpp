#include <mouse_position.h>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

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

const glm::ivec2 MousePosition::on_screen() const {
    return m_mouse_window_current_position;
}

const glm::ivec2 MousePosition::in_world() const {
    return m_mouse_window_current_position + camera_position;
}

const glm::ivec2 MousePosition::on_grid() const {
    return tilemap.world_px_to_grid(in_world());
}