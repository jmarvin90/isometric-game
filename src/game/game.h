#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <unordered_map>

#include "../components/transform.h"
#include "../components/sprite.h"

constexpr int FPS               {60};
constexpr int MILLIS_PER_FRAME  {1'000/FPS};
constexpr int WINDOW_HEIGHT     {768};
constexpr int WINDOW_WIDTH      {1024};
constexpr int TILE_HEIGHT       {60};
constexpr int TILE_WIDTH        {120};
constexpr int MAP_SIZE          {5};
constexpr int TILEMAP_X_START   {(WINDOW_WIDTH / 2) - (TILE_WIDTH / 2)};
constexpr int TILEMAP_Y_START   {100};

class TileMap {
    std::vector<std::vector<entt::entity>> tilemap;

    public:
        entt::entity at(const int x, const int y);
        TileMap(entt::registry& registry);
        ~TileMap();
};

class Game {
    bool is_running {false};
    bool debug_mode {false};

    // Has to be default initialised because it's referenced in Game::update()
    int millis_previous_frame{};    

    // TODO: the EnTT registry
    entt::registry registry;

    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_Rect camera{}; // Investigate whether this should be default-initialised
    // Todo: read re. asset stores
    std::unordered_map<int, SDL_Texture*> textures;

    TileMap tilemap;

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