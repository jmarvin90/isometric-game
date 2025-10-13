#include <scene.h>
#include <spdlog/spdlog.h>

void Scene::update() {

    mouse_position.update();

    if (
        mouse_position.on_screen().x < scene_border_px &&
        camera_position.x > 0
    ) {
        camera_position.x -= 4;
    }

    if (
        (display_mode.w - mouse_position.on_screen().x) < scene_border_px &&
        (camera_position.x + display_mode.w) < tilemap.area().x
    ) {
        camera_position.x += 4;
    }

    if (
        mouse_position.on_screen().y < scene_border_px &&
        camera_position.y > 0
    ) {
        camera_position.y -= 4;
    }

    if (
        (display_mode.h - mouse_position.on_screen().y) < scene_border_px &&
        (camera_position.y + display_mode.h) < tilemap.area().y
    ) {
        camera_position.y += 4;
    }


    if (mouse_position.moved()) {
        spdlog::info(
            "Mouse position: " + 
            std::to_string(mouse_position.on_grid().x) + "," + 
            std::to_string(mouse_position.on_grid().y)
        );
        tilemap.highlight_tile(mouse_position.on_grid());
    }
}