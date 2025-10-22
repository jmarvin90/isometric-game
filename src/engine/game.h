#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <unordered_map>
#include <memory>
#include <optional>

#include <render.h>
#include <components/transform.h>
#include <constants.h>
#include <scene.h>
#include <spritesheet.h>

class Game {
    bool is_running {false};
    bool debug_mode {false};

    // Has to be default initialised because it's referenced in Game::update()
    int millis_previous_frame{};    
    uint64_t _last_time{0};

    SDL_Window* window;
    SDL_DisplayMode display_mode;

    // Camera, renderer are smart pointers to allow late initialisation
    std::optional<Renderer> renderer;
    std::optional<Scene> scene;
    std::optional<SpriteSheet> spritesheet;

    void process_input();
    void update(const float delta_time);
    void render();

    public:

        Game();
        ~Game();

        Game(const Game&) = delete;
        Game operator=(const Game&) = delete;

        void initialise();
        void run();
        void destroy();
        
};

#endif