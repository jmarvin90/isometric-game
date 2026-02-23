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
    // SDL_Rect camera_rect;
    SpatialMapCellSpanComponent spatial_map_cell_span;
    bool moved_in_frame;

    CameraComponent(const SDL_DisplayMode& display_mode, const Grid<SpatialMapProjection>& spatial_map)
        : position { 0, 0 }
        , size { display_mode.w, display_mode.h }
        //     : camera_rect { 0, 0, display_mode.w, display_mode.h }
        , spatial_map_cell_span {
            SpatialMapProjection::world_to_grid({ 0, 0 }, spatial_map),
            SpatialMapProjection::world_to_grid({ display_mode.w, display_mode.h }, spatial_map)
        }
        , moved_in_frame { false } { };
};

#endif