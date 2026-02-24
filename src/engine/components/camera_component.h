#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

struct CameraComponent {
    glm::ivec2 position;
    glm::ivec2 size;
    bool moved_in_frame;

    CameraComponent(const SDL_DisplayMode& display_mode)
        : position { 0, 0 }
        , size { display_mode.w, display_mode.h }
        , moved_in_frame { false } { };
};

#endif