#include "camera.h"
#include "constants.h"

void Camera::update(
    const glm::ivec2& mouse_screen_position
) {
    if (
        mouse_screen_position.x < constants::CAMERA_BORDER_PX &&
        camera_position.x > 0
    ) {
        camera_position.x -= 4;
    }

    if (
        (constants::WINDOW_WIDTH - mouse_screen_position.x) < constants::CAMERA_BORDER_PX &&
        (camera_position.x + camera_position.w) < constants::RENDER_SPACE_WIDTH_PX
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
        (constants::WINDOW_HEIGHT - mouse_screen_position.y) < constants::CAMERA_BORDER_PX &&
        (camera_position.y + camera_position.h) < constants::RENDER_SPACE_HEIGHT_PX
    ) {
        camera_position.y += 4;
    }
}

const SDL_Rect& Camera::get_position() const {
    return camera_position;
}