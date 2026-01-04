#include <components/camera_component.h>
#include <components/mouse_component.h>
#include <components/tilemap_component.h>
#include <components/transform_component.h>
#include <constants.h>
#include <systems/camera_system.h>

void CameraSystem::update(entt::registry& registry, const SDL_DisplayMode& display_mode)
{
    const MouseComponent& mouse { registry.ctx().get<const MouseComponent>() };
    CameraComponent& camera { registry.ctx().get<CameraComponent>() };
    const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };
    glm::ivec2 camera_previous_pos { camera.position() };

    if (mouse.window_current_position.x < constants::SCENE_BORDER_PX.x && camera.camera_rect.x > 0) {
        camera.camera_rect.x -= constants::CAMERA_SCROLL_SPEED.x;
    }

    if (mouse.window_current_position.y < constants::SCENE_BORDER_PX.y && camera.camera_rect.y > 0) {
        camera.camera_rect.y -= constants::CAMERA_SCROLL_SPEED.y;
    }

    if ((display_mode.w - mouse.window_current_position.x) < constants::SCENE_BORDER_PX.x
        && (camera.camera_rect.x + display_mode.w) < (tilemap.area.x + (constants::SCENE_BORDER_PX.x * 2))) {
        camera.camera_rect.x += constants::CAMERA_SCROLL_SPEED.x;
    }

    if ((display_mode.h - mouse.window_current_position.y) < constants::SCENE_BORDER_PX.y
        && (camera.camera_rect.y + display_mode.h) < (tilemap.area.y + (constants::SCENE_BORDER_PX.y * 2))) {
        camera.camera_rect.y += constants::CAMERA_SCROLL_SPEED.y;
    }
}