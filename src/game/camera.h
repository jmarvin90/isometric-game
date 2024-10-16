#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "constants.h"

class Camera {
    SDL_Rect camera_position {0, 0, constants::WINDOW_WIDTH, constants::WINDOW_HEIGHT};

    public:
        void set_position(const glm::ivec2& mouse_position);
        const SDL_Rect& get_position() const;
};

#endif