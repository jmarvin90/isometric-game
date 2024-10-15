#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <unordered_map>

#include "../components/transform.h"
#include "../components/sprite.h"
#include "tilemap.h"
#include "mouse.h"

class Game {
    bool is_running {false};
    bool debug_mode {false};

    // Has to be default initialised because it's referenced in Game::update()
    int millis_previous_frame{};    

    entt::registry registry;
    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_Rect camera{}; // Investigate whether this should be default-initialised
    
    // Todo: read re. asset stores
    std::unordered_map<int, SDL_Texture*> textures;

    TileMap tilemap;
    Mouse mouse;

    void load_textures();
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

        void initialise();
        void run();
        void destroy();
};

#endif