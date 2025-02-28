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
#include "spritesheet.h"
#include "systems/render.h"

class Game {
    bool is_running {false};
    bool debug_mode {false};

    // Has to be default initialised because it's referenced in Game::update()
    int millis_previous_frame{};    
    uint64_t _last_time{0};

    entt::registry registry;
    SDL_Window* window;
    SDL_DisplayMode display_mode;

    // Camera, renderer are smart pointers to allow late initialisation
    std::optional<Camera> camera;
    std::optional<Renderer> renderer;

    // TileMap and Mouse can be initialised during game construction
    TileMap tilemap;
    MouseMap mousemap;
    Mouse mouse;

    void load_spritesheets();
    void load_tilemap();
    void process_input();
    void update(const float delta_time);
    void render();

    public:

        // Todo: read re. asset stores; make private if necessary
        std::optional<SpriteSheet> city_tiles;
        std::optional<SpriteSheet> building_tiles;

        Game();
        ~Game();

        // TODO: define copy constructor to enable -Weffc++
        // Game(const Game&) = ...;

        // TODO: define operator= method to enable -Weffc++
        // ...operator=(const Game&) ...;

        void initialise();
        void run();
        void destroy();

        const TileMap& get_tilemap() {
            return tilemap;
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