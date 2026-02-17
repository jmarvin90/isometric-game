#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>
#include <components/spatialmap_component.h>
#include <components/spatialmapcell_span_component.h>
#include <glm/glm.hpp>
#include <position.h>

struct CameraComponent {
    SDL_Rect camera_rect;
    SpatialMapCellSpanComponent spatial_map_cell_span;
    bool moved_in_frame;

    CameraComponent(const SDL_DisplayMode& display_mode, const SpatialMapComponent& spatial_map)
        : camera_rect { 0, 0, display_mode.w, display_mode.h }
        , spatial_map_cell_span {
            Position::world_to_spatial_map({ 0, 0 }, spatial_map.cell_size),
            Position::world_to_spatial_map({ display_mode.w, display_mode.h }, spatial_map.cell_size)
        }
        , moved_in_frame { false } { };
    glm::ivec2 position() const { return glm::ivec2 { camera_rect.x, camera_rect.y }; }
};

#endif