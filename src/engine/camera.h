#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "constants.h"

class Camera {
    SDL_Rect camera_position;

    public:
        Camera(const SDL_DisplayMode& display_mode);
        ~Camera() = default;

        void update(const SDL_DisplayMode& display_mode, const glm::ivec2& mouse_screen_position);
        const SDL_Rect& get_position() const;
};

#endif