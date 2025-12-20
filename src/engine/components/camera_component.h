#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

struct CameraComponent {
    SDL_Rect camera_rect;
    bool moved_in_frame;
    CameraComponent(const SDL_DisplayMode& display_mode)
    : camera_rect {0, 0, display_mode.w, display_mode.h}
    , moved_in_frame {false} 
    {};
    glm::ivec2 position () const { return glm::ivec2{camera_rect.x, camera_rect.y}; }
};

#endif