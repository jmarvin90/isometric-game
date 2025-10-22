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