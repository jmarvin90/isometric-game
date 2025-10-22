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

    public:
        TileMap tilemap;
        MousePosition mouse_position;
        friend class Renderer;
        const int n_tiles;

        Scene(
            const SpriteSheet& spritesheet,
            const SDL_DisplayMode& display_mode,
            const glm::ivec2 tile_size,
            const int n_tiles,
            const int scene_border_px
        )
        : spritesheet {spritesheet}
        , registry {entt::registry()}
        , tilemap {registry, n_tiles, tile_size, spritesheet}
        , scene_border_px {scene_border_px}
        , display_mode {display_mode}
        , n_tiles {n_tiles}
        , mouse_position {tilemap, camera_position, scene_border_px}
        {}

        Scene(const Scene&) = delete;
        ~Scene() = default;

        void update();
};

#endif