#include <scene.h>
#include <spdlog/spdlog.h>

void Scene::update() {
    m_mouse_window_previous_position = m_mouse_window_current_position;

    mouse_state = SDL_GetMouseState(
        &m_mouse_window_current_position.x,
        &m_mouse_window_current_position.y
    );

    if (
        m_mouse_window_current_position.x < scene_border_px &&
        camera_position.x > 0
    ) {
        camera_position.x -= 4;
    }

    if (
        (display_mode.w - m_mouse_window_current_position.x) < scene_border_px &&
        (camera_position.x + display_mode.w) < tilemap.area().x
    ) {
        camera_position.x += 4;
    }

    if (
        m_mouse_window_current_position.y < scene_border_px &&
        camera_position.y > 0
    ) {
        camera_position.y -= 4;
    }

    if (
        (display_mode.h - m_mouse_window_current_position.y) < scene_border_px &&
        (camera_position.y + display_mode.h) < tilemap.area().y
    ) {
        camera_position += 4;
    }

    m_mouse_world_position = m_mouse_window_current_position + camera_position;

    if (m_mouse_window_current_position != m_mouse_window_previous_position) {
        m_highlighted_tile = tilemap[m_mouse_world_position];
    }
}