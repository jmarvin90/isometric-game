#include "camera.h"
#include "constants.h"
#include <SDL2/SDL.h>

Camera::Camera(const SDL_DisplayMode& display_mode)
    : camera_area{
        (constants::RENDER_SPACE_SIZE_PX.x / 2) - (display_mode.w / 2), // Initial X
        (constants::RENDER_SPACE_SIZE_PX.y / 2) - (display_mode.h / 2), // Initial Y
        display_mode.w,                                                 // Width
        display_mode.h                                                  // Height
    }
{
}

void Camera::update(
    const SDL_DisplayMode& display_mode,
    const glm::ivec2& mouse_screen_position)
{
    /* Move the camera in a given direction if:
        - There's space in that direction; and
        - The camera is in the border facing that direction
    */

    // Move left
    if (
        mouse_screen_position.x < constants::CAMERA_BORDER_PX &&
        camera_area.x > 0)
    {
        camera_area.x -= 4;
    }

    // Move right
    if (
        (display_mode.w - mouse_screen_position.x) < constants::CAMERA_BORDER_PX &&
        (camera_area.x + camera_area.w) < constants::RENDER_SPACE_SIZE_PX.x)
    {
        camera_area.x += 4;
    }

    // Move up
    if (
        mouse_screen_position.y < constants::CAMERA_BORDER_PX &&
        camera_area.y > 0)
    {
        camera_area.y -= 4;
    }

    // Move down
    if (
        (display_mode.h - mouse_screen_position.y) < constants::CAMERA_BORDER_PX &&
        (camera_area.y + camera_area.h) < constants::RENDER_SPACE_SIZE_PX.y)
    {
        camera_area.y += 4;
    }
}

const glm::ivec2 Camera::get_position() const
{
    return glm::ivec2{ camera_area.x, camera_area.y };
}