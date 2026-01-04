#include <SDL2/SDL_image.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <components/navigation_component.h>
#include <components/tilemap_component.h>
#include <components/tilespec_component.h>
#include <components/segment_component.h>
#include <components/segment_manager_component.h>
#include <constants.h>
#include <game.h>
#include <imgui.h>
#include <spatialmap_component.h>
#include <spdlog/spdlog.h>
#include <spritesheet.h>
#include <systems/camera_system.h>
#include <systems/mouse_system.h>
#include <systems/render_system.h>
#include <systems/spatialmap_system.h>
#include <systems/tilemap_system.h>
#include <systems/segment_system.h>

#include <entt/entt.hpp>
#include <string>

Game::Game() { spdlog::info("Game constructor called."); }

Game::~Game() { spdlog::info("Game destructor called."); }

void Game::initialise()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GetDesktopDisplayMode(0, &display_mode);

    // Create the SDL Window
    window = SDL_CreateWindow(NULL, // title - leave blank for now
                              SDL_WINDOWPOS_CENTERED, // Window xconstant position (centred)
                              SDL_WINDOWPOS_CENTERED, // Window y position (centred)
                              display_mode.w, // X res from current display mode
                              display_mode.h, // Y res from current display mode
                              SDL_WINDOW_FULLSCREEN // | SDL_WINDOW_ALLOW_HIGHDPI // Input grabbed flag
    );

    if (!window) {
        spdlog::error("Could not initialise SDL Window.");
    }

    registry = entt::registry();

    registry.on_construct<NavigationComponent>().connect<&TileMapSystem::connect>();

    // TODO: move this somewhere smart under some smart condition
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    registry.ctx().emplace<MouseComponent>();
    registry.ctx().emplace<CameraComponent>(display_mode);
    registry.ctx().emplace<TileSpecComponent>(256, 14);
    registry.ctx().emplace<SpriteSheet>(
        std::string { "assets/spritesheet_scaled.png" },
        std::string { "assets/spritesheet.json" },
        renderer
    );
    registry.ctx().emplace<TileMapComponent>(registry, 8);
    registry.ctx().emplace<SpatialMapComponent>(registry, 4);
    registry.ctx().emplace<SegmentManagerComponent>();

    registry.on_construct<NavigationComponent>().connect<&TileMapSystem::connect>();
    registry.on_construct<SpriteComponent>().connect<&SpatialMapSystem::register_entity>();
    registry.on_construct<SegmentComponent>().connect<&SegmentSystem::connect>();
    registry.on_destroy<SegmentComponent>().connect<&SegmentSystem::disconnect>();

    TileMapSystem::emplace_tiles(registry);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
}

void Game::process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                is_running = false;
            }

            if (event.key.keysym.sym == SDLK_d) {
                debug_mode = !debug_mode;
            }

            break;

        case SDL_MOUSEBUTTONDOWN:
            break;
        }
    }
}

void Game::update([[maybe_unused]] const float delta_time)
{
    MouseSystem::update(registry);
    CameraSystem::update(registry, display_mode);
    TileMapSystem::update(registry, debug_mode);
    SegmentSystem::update(registry);
    RenderSystem::update(registry);
}

void Game::render()
{
    // SDL_RenderSetClipRect(renderer, &camera.camera_rect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    RenderSystem::render(registry, renderer);
    if (debug_mode) {
        RenderSystem::render_highlights(registry, renderer);
        RenderSystem::render_imgui_ui(registry, renderer);
        RenderSystem::render_segment_lines(registry, renderer);
    }
    SDL_RenderPresent(renderer);
}

void Game::run()
{
    is_running = true;
    while (is_running) {
        // The start point (in ticks), the delta to the last frame in s/ms
        const uint64_t start { SDL_GetTicks64() };
        const uint64_t since_last_frame { start - _last_time };
        const float delta_time = { since_last_frame / 1'000.f };

        process_input();
        update(delta_time);
        render();

        // How many millis have elapsed this frame
        const uint64_t elapsed_this_frame { SDL_GetTicks64() - start };

        // Delay until the START of the next frame
        const float time_to_delay { constants::MILLIS_PER_FRAME - elapsed_this_frame };

        if (time_to_delay > 0 && time_to_delay <= constants::MILLIS_PER_FRAME) {
            SDL_Delay(time_to_delay);
        }

        _last_time = start;
    }
}

void Game::destroy()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyWindow(window);
    SDL_Quit();
}