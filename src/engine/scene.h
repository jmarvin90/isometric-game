#ifndef SCENE_H
#define SCENE_H

#include <tilemap.h>
#include <SDL2/SDL.h>
#include <optional>
#include <systems/render.h>
#include <tilespec.h>
#include <mouse_position.h>

class Scene {
    private:
        entt::registry registry;
        TileMap tilemap;
        glm::ivec2 camera_position {0,0};
        const int scene_border_px;
        const SDL_DisplayMode& display_mode;
        MousePosition mouse_position;

    public:
        friend class Renderer;
        const int n_tiles;

        Scene(
            const SDL_DisplayMode& display_mode,
            const TileSpec tile_spec,
            const int n_tiles,
            const int scene_border_px
        )
        : registry {entt::registry()}
        , tilemap {registry, n_tiles, tile_spec}
        , scene_border_px {scene_border_px}
        , display_mode {display_mode}
        , n_tiles {n_tiles}
        , mouse_position {tilemap, camera_position, scene_border_px}
        {}

        Scene(const Scene&) = delete;
        ~Scene() = default;

        void update();
        const entt::registry& get_registry() const { return registry; }
        const int get_border_px() const { return scene_border_px; }
};

#endif