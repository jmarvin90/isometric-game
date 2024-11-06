#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "constants.h"

class Camera {
    SDL_Rect camera_position {
        (constants::RENDER_SPACE_WIDTH_PX / 2) - (constants::WINDOW_WIDTH / 2),     // Initial X
        0,                                                                          // Initial Y
        constants::WINDOW_WIDTH,                                                    // Width
        constants::WINDOW_HEIGHT                                                    // Height
    };

    public:
        void update(const glm::ivec2& mouse_screen_position);
        const SDL_Rect& get_position() const;
};

#endif