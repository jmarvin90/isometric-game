#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <unordered_map>

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

    entt::registry registry;
    SDL_Renderer* renderer;
    SDL_Window* window;

    // Investigate whether this should be default-initialised
    Camera camera;

    // Investigate whether this is redundant!
    SDL_Rect render_rect{20, 20, constants::WINDOW_WIDTH-40, constants::WINDOW_HEIGHT-40};
    
    // Todo: read re. asset stores
    std::unordered_map<int, SDL_Texture*> textures;

    TileMap tilemap;
    Mouse mouse;

    void load_textures(const std::vector<std::string>& tile_paths);
    void load_tilemap();
    void process_input();
    void update();
    void render();

    public:
        Game();
        ~Game();

        // TODO: define copy constructor to enable -Weffc++
        // Game(const Game&) = ...;

        // TODO: define operator= method to enable -Weffc++
        // ...operator=(const Game&) ...;

        void initialise(const std::vector<std::string>& tile_paths);
        void run();
        void destroy();

        const TileMap& get_tilemap() {
            return tilemap;
        }

        SDL_Texture* fetch_texture(int index) {
            return textures[index];
        }

        entt::entity create_entity();

        template <typename T, typename ...TArgs>
        void add_component(const entt::entity& entity, TArgs ...args) {
            registry.emplace<T>(entity, std::forward<TArgs>(args)...);
        }
};

#endif