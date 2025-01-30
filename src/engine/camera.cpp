#include "camera.h"
#include "constants.h"
#include <SDL2/SDL.h>

Camera::Camera(const SDL_DisplayMode& display_mode): camera_position {
    (constants::RENDER_SPACE_SIZE_PX.x / 2) - (display_mode.w / 2),             // Initial X
    0,                                                                          // Initial Y
    display_mode.w,                                                             // Width
    display_mode.h                                                              // Height
} {}

void Camera::update(
    const SDL_DisplayMode& display_mode,
    const glm::ivec2& mouse_screen_position
) {
    if (
        mouse_screen_position.x < constants::CAMERA_BORDER_PX &&
        camera_position.x > 0
    ) {
        camera_position.x -= 4;
    }

    if (
        (display_mode.w - mouse_screen_position.x) < constants::CAMERA_BORDER_PX &&
        (camera_position.x + camera_position.w) < constants::RENDER_SPACE_SIZE_PX.x
    ) {
        camera_position.x += 4;
    }

    if (
        mouse_screen_position.y < constants::CAMERA_BORDER_PX &&
        camera_position.y > 0
    ) {
        camera_position.y -= 4;
    }

    if (
        (display_mode.h - mouse_screen_position.y) < constants::CAMERA_BORDER_PX &&
        (camera_position.y + camera_position.h) < constants::RENDER_SPACE_SIZE_PX.y
    ) {
        camera_position.y += 4;
    }
}

const SDL_Rect& Camera::get_position() const {
    return camera_position;
}