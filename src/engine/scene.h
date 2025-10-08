#ifndef SCENE_H
#define SCENE_H

#include <tilemap.h>
#include <SDL2/SDL.h>
#include <optional>

#include <systems/render.h>

class Scene {
    private:
        entt::registry registry;
        TileMap tilemap;
        glm::ivec2 camera_position {0,0};
        const int scene_border_px;
        const SDL_DisplayMode& display_mode;
        
        // Mouse info
        uint32_t mouse_state;
        std::optional<glm::ivec2> m_mouse_window_previous_position;
        glm::ivec2 m_mouse_window_current_position;
        glm::ivec2 m_mouse_world_position;

        // Tilemap info
        Tile* m_highlighted_tile;


    public:
        friend class Renderer;

        Scene(
            const SDL_DisplayMode& display_mode,
            const glm::ivec2 tile_spec,
            const int n_tiles,
            const int scene_border_px
        )
        : registry {entt::registry()}
        , tilemap {registry, n_tiles, tile_spec}
        , scene_border_px {scene_border_px}
        , display_mode {display_mode}
        , m_highlighted_tile {nullptr}
        {}

        Scene(const Scene&) = delete;
        ~Scene() = default;

        void update();
        const entt::registry& get_registry() const { return registry; }
        const int get_border_px() const { return scene_border_px; }
        const glm::ivec2& mouse_world_position() const { return m_mouse_world_position; }
        Tile* highlighted_tile() { return m_highlighted_tile; }

        // For testing - to be deleted
        std::vector<Tile>& get_tiles() { return tilemap.get_tiles(); }
};

#endif