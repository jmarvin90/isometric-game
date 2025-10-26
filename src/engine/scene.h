#ifndef SCENE_H
#define SCENE_H

#include <tilemap.h>
#include <SDL2/SDL.h>
#include <optional>
#include <render.h>
#include <mouse_position.h>
#include <spritesheet.h>

class Scene {
    private:
        entt::registry registry;
        const SpriteSheet& spritesheet;
        glm::ivec2 camera_position {0,0};
        const int scene_border_px;
        const SDL_DisplayMode& display_mode;
        int scroll_speed;

    public:
        TileMap tilemap;
        MousePosition mouse_position;
        friend class Renderer;
        const int n_tiles;

        Scene(
            const SpriteSheet& spritesheet,
            const SDL_DisplayMode& display_mode,
            const int tile_width,
            const int tile_depth,
            const int n_tiles,
            const int scene_border_px,
            const int scroll_speed
        )
        : spritesheet {spritesheet}
        , registry {entt::registry()}
        , tilemap {registry, n_tiles, tile_width, tile_depth, spritesheet}
        , scene_border_px {scene_border_px}
        , display_mode {display_mode}
        , n_tiles {n_tiles}
        , mouse_position {tilemap, camera_position, scene_border_px}
        , scroll_speed {scroll_speed}
        {}

        Scene(const Scene&) = delete;
        ~Scene() = default;

        void update();
        void set_scroll_speed(const int new_speed) { scroll_speed = new_speed; }

        void create_building_at(
            const glm::ivec2 grid_position,
            const std::string building_sprite_name
        );
};

#endif