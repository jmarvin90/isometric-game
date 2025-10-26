#include <scene.h>
#include <spdlog/spdlog.h>

void Scene::update() {

    mouse_position.update();

    const glm::ivec2 mouse_screen_position {mouse_position.on_screen()};

    if (
        mouse_screen_position.x < scene_border_px &&
        camera_position.x > 0
    ) {
        camera_position.x -= scroll_speed;
    }

    if (
        (display_mode.w - mouse_screen_position.x) < scene_border_px &&
        (camera_position.x + display_mode.w) < (tilemap.area().x + scene_border_px * 2)
    ) {
        camera_position.x += scroll_speed;
    }

    if (
        mouse_screen_position.y < scene_border_px &&
        camera_position.y > 0
    ) {
        camera_position.y -= scroll_speed;
    }

    if (
        (display_mode.h - mouse_screen_position.y) < scene_border_px &&
        (camera_position.y + display_mode.h) < (tilemap.area().y + scene_border_px * 2)
    ) {
        camera_position.y += scroll_speed;
    }


    if (mouse_position.moved()) {
        tilemap.highlight_tile(mouse_position.on_grid());
    }
}

void Scene::create_building_at(
    const glm::ivec2 grid_position,
    const std::string building_sprite_name
) {
    Tile* tile { tilemap[grid_position] };

    if (!tile) {
        return;
    }

    if (!tile->building_entity) {
        tile->building_entity.emplace(registry.create());
    }

    std::remove_const_t<Transform> transform {
        registry.get<Transform>(tile->tile_entity)
    };

    Sprite building_sprite {spritesheet.get(building_sprite_name)};

    int vertical_offset {
        building_sprite.source_rect.h - 
        tilemap.tile_spec().iso_area.y
    };

    transform.position.y -= vertical_offset;
    transform.z_index = 1;

    registry.emplace<Sprite>(
        tile->building_entity.value(),
        spritesheet.get(building_sprite_name)
    );

    registry.emplace<Transform>(
        tile->building_entity.value(),
        transform
    );
}