#include <cmath>
#include <components/camera_component.h>
#include <components/mouse_component.h>
#include <constants.h>
#include <grid.h>
#include <projection.h>
#include <systems/camera_system.h>

void CameraSystem::update(entt::registry& registry)
{
    const MouseComponent& mouse { registry.ctx().get<const MouseComponent>() };
    CameraComponent& camera { registry.ctx().get<CameraComponent>() };
    const Grid<TileMapProjection>& tilemap { registry.ctx().get<const Grid<TileMapProjection>>() };
    camera.moved_in_frame = false;

    glm::ivec2 map_extent { tilemap.area + (constants::SCENE_BORDER_PX * 2) };
    glm::ivec2 delta { 0, 0 };

    for (int i = 0; i < 2; i++) {
        if (
            mouse.window_current_position[i] < constants::SCENE_BORDER_PX[i]
            && camera.position[i] > 0
        ) {
            delta[i] = std::max(
                -constants::CAMERA_SCROLL_SPEED[i],
                -mouse.window_current_position[i]
            );
        } else if (
            mouse.window_current_position[i] > (camera.size - constants::SCENE_BORDER_PX)[i]
            && (camera.position + camera.size)[i] < map_extent[i]
        ) {
            delta[i] = std::min(
                constants::CAMERA_SCROLL_SPEED[i],
                map_extent[i] - mouse.window_current_position[i]
            );
        }
    }

    if (delta != glm::ivec2 { 0, 0 }) {
        camera.moved_in_frame = true;
        camera.position += delta;
    }

    if (camera.moved_in_frame) {
        const Grid<SpatialMapProjection>& spatial_map { registry.ctx().get<const Grid<SpatialMapProjection>>() };
        camera.spatial_map_cell_span.AA = SpatialMapProjection::world_to_grid(camera.position, spatial_map);
        camera.spatial_map_cell_span.BB = SpatialMapProjection::world_to_grid(camera.position + camera.size, spatial_map);
    }
}