#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "constants.h"

class Camera
{
    SDL_Rect camera_area;

public:
    Camera(const SDL_DisplayMode &display_mode);
    ~Camera() = default;

    void update(const SDL_DisplayMode &display_mode, const glm::ivec2 &mouse_screen_position);
    const glm::ivec2 get_position() const;
};

#endif