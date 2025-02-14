#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <unordered_map>
#include <memory>
#include <optional>

#include "components/transform.h"
#include "components/sprite.h"
#include "tilemap.h"
#include "mouse.h"
#include "camera.h"
#include "constants.h"

class Game {
    bool is_running {false};
    bool debug_mode {false};

    // Has to be default initialised because it's referenced in Game::update()
    int millis_previous_frame{};    
    uint64_t _last_time{0};

    entt::registry registry;
    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_DisplayMode display_mode;

    // Camera is smart pointer to allow late initialisation
    std::optional<Camera> camera;

    // TileMap and Mouse can be initialised during game construction
    TileMap tilemap;
    MouseMap mousemap;
    Mouse mouse;

    // Investigate whether this is redundant!
    SDL_Rect render_rect;
    
    // Todo: read re. asset stores
    std::unordered_map<std::string, SDL_Texture*> textures;

    void load_textures(const std::string& directory);
    void load_tilemap();
    void process_input();
    void update(const float delta_time);
    void render();

    public:
        Game();
        ~Game();

        // TODO: define copy constructor to enable -Weffc++
        // Game(const Game&) = ...;

        // TODO: define operator= method to enable -Weffc++
        // ...operator=(const Game&) ...;

        void initialise(const std::string textures_path);
        void run();
        void destroy();

        const TileMap& get_tilemap() {
            return tilemap;
        }

        SDL_Texture* const fetch_texture(const std::string& index) {
            return textures[index];
        }

        entt::entity create_entity();

        template <typename T, typename ...TArgs>
        void add_component(const entt::entity& entity, TArgs ...args) {
            registry.emplace<T>(entity, std::forward<TArgs>(args)...);
        }

        template <typename T>
        T get_component(const entt::entity& entity) {
            return registry.get<T>(entity);
        }
};

#endif