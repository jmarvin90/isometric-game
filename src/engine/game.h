#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <iso_utility.h>
#include <memory>

template <typename Flag>
void flag(entt::registry& registry, entt::entity entity)
{
    registry.emplace_or_replace<Flag>(entity);
}

class Game {
    bool is_running { false };
    bool debug_mode { false };

    // Has to be default initialised because it's referenced in Game::update()
    int millis_previous_frame {};
    uint64_t _last_time { 0 };

    std::unique_ptr<SDL_Window, ISOUtility::SDLDestroyer> window;
    // SDL_Window* window;
    SDL_DisplayMode display_mode;

    // CameraComponent, renderer are smart pointers to allow late initialisation
    entt::registry registry;
    std::unique_ptr<SDL_Renderer, ISOUtility::SDLDestroyer> renderer;
    // SDL_Renderer* renderer;

    void process_input();
    void update(const float delta_time);
    void render();

public:
    Game();
    ~Game();

    void initialise();
    void run();
    void destroy();
};

#endif