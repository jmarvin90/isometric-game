#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>
#include <components/spatialmapcell_span_component.h>
#include <glm/glm.hpp>
#include <grid.h>
#include <position.h>
#include <projection.h>

struct CameraComponent {
    glm::ivec2 position;
    glm::ivec2 size;
    bool moved_in_frame;

    CameraComponent(const SDL_DisplayMode& display_mode, const Grid<SpatialMapProjection>& spatial_map)
        : position { 0, 0 }
        , size { display_mode.w, display_mode.h }
        , moved_in_frame { false } { };
};

#endif