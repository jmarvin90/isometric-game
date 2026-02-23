#include <components/camera_component.h>
#include <components/mouse_component.h>
#include <constants.h>
#include <grid.h>
#include <projection.h>
#include <systems/camera_system.h>

void CameraSystem::update(entt::registry& registry, const SDL_DisplayMode& display_mode)
{
    const MouseComponent& mouse { registry.ctx().get<const MouseComponent>() };
    CameraComponent& camera { registry.ctx().get<CameraComponent>() };
    const Grid<TileMapProjection>& tilemap { registry.ctx().get<const Grid<TileMapProjection>>() };
    camera.moved_in_frame = false;

    if (mouse.window_current_position.x < constants::SCENE_BORDER_PX.x && camera.camera_rect.x > 0) {
        camera.camera_rect.x -= constants::CAMERA_SCROLL_SPEED.x;
        camera.moved_in_frame = true;
    }

    if (mouse.window_current_position.y < constants::SCENE_BORDER_PX.y && camera.camera_rect.y > 0) {
        camera.camera_rect.y -= constants::CAMERA_SCROLL_SPEED.y;
        camera.moved_in_frame = true;
    }

    if ((display_mode.w - mouse.window_current_position.x) < constants::SCENE_BORDER_PX.x
        && (camera.camera_rect.x + display_mode.w) < (tilemap.area.x + (constants::SCENE_BORDER_PX.x * 2))) {
        camera.camera_rect.x += constants::CAMERA_SCROLL_SPEED.x;
        camera.moved_in_frame = true;
    }

    if ((display_mode.h - mouse.window_current_position.y) < constants::SCENE_BORDER_PX.y
        && (camera.camera_rect.y + display_mode.h) < (tilemap.area.y + (constants::SCENE_BORDER_PX.y * 2))) {
        camera.camera_rect.y += constants::CAMERA_SCROLL_SPEED.y;
        camera.moved_in_frame = true;
    }

    if (camera.moved_in_frame) {
        const Grid<SpatialMapProjection>& spatial_map { registry.ctx().get<const Grid<SpatialMapProjection>>() };
        glm::ivec2 AA { camera.position() };
        glm::ivec2 BB { camera.position() + glm::ivec2 { camera.camera_rect.w, camera.camera_rect.h } };
        camera.spatial_map_cell_span.AA = SpatialMapProjection::world_to_grid(AA, spatial_map);
        camera.spatial_map_cell_span.BB = SpatialMapProjection::world_to_grid(BB, spatial_map);
    }
}