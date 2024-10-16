#include "camera.h"
#include "constants.h"

void Camera::set_position(const glm::ivec2& mouse_position) {
    if (mouse_position.x < constants::CAMERA_BORDER_PX) {
        --camera_position.x;
    }

    if (
        (constants::WINDOW_WIDTH - mouse_position.x) < 
        constants::CAMERA_BORDER_PX
    ) {
        ++camera_position.x;
    }

    if (mouse_position.y < constants::CAMERA_BORDER_PX) {
        --camera_position.y;
    }

    if (
        (constants::WINDOW_HEIGHT - mouse_position.y) <
        constants::CAMERA_BORDER_PX
    ) {
        ++camera_position.y;
    }
}

const SDL_Rect& Camera::get_position() const {
    return camera_position;
}