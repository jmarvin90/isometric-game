#include <string>

#include <glm/glm.hpp>
#include <SDL2/SDL_image.h>
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

#include "constants.h"
#include "game.h"

#include "components/transform.h"
#include "components/sprite.h"
#include "components/rigid_body.h"

#include "systems/movement.h"
#include "systems/render.h"
#include "systems/imgui_render.h"
#include "systems/bounding_box_render.h"

Game::Game(): 
    registry{entt::registry()}, 
    tilemap{registry},
    mousemap{"./assets/mousemap.png"},
    mouse{mousemap}
{
    spdlog::info("Game constructor called.");
}

Game::~Game() {
    spdlog::info("Game destuctor called.");
}

void Game::load_textures(const std::vector<std::string>& tile_paths){

    for (unsigned int texture_id=0; texture_id<tile_paths.size(); texture_id++) {
        SDL_Surface* surface {IMG_Load(tile_paths.at(texture_id).c_str())};
        if (!surface) {
            spdlog::info(
                "Could not load texture from path: " +
                tile_paths.at(texture_id)
            );
        }

        SDL_Texture* texture {SDL_CreateTextureFromSurface(renderer, surface)};
        if (!texture) {
            spdlog::info(
                "Could not load texture from surface using image: " +
                tile_paths.at(texture_id)
            );
        }

        SDL_FreeSurface(surface);
        textures.emplace(texture_id, texture);
    }
}

void Game::load_tilemap() {
    spdlog::info("Loading tilemap");
    for (int y=0; y<constants::MAP_SIZE_N_TILES; y++) {
        for (int x=0; x<constants::MAP_SIZE_N_TILES; x++) {

            glm::ivec2 position {tilemap.grid_to_pixel(x, y)};
            int texture_id {1};

            entt::entity entity {tilemap.at(x, y)};
            
            registry.emplace<Transform>(entity, position, 0.0);
            registry.emplace<TerrainSprite>(entity, textures[texture_id]);
        }
    }
}

entt::entity Game::create_entity() {
    return registry.create();
}

void Game::initialise(const std::vector<std::string>& tile_paths) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GetDesktopDisplayMode(0, &display_mode);
    camera = std::make_unique<Camera>(display_mode);

    // Create the SDL Window
    window = SDL_CreateWindow(
        NULL,                       // title - leave blank for now
        SDL_WINDOWPOS_CENTERED,     // Window xconstant position (centred)
        SDL_WINDOWPOS_CENTERED,     // Window y position (centred)
        display_mode.w,             // X res from current display mode
        display_mode.h,             // Y res from current display mode
        SDL_WINDOW_FULLSCREEN       // Input grabbed flag
    );

    if (!window) {
        spdlog::error("Could not initialise SDL Window.");
    }

    // Create the SDL Renderer
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        spdlog::error("Could not initialise the SDL Renderer.");
    }

    load_textures(tile_paths);
    load_tilemap();

    render_rect = {
        20, 
        20,
        display_mode.w - 40,
        display_mode.h - 40
    };

    SDL_RenderSetClipRect(renderer, &render_rect);    

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
}

void Game::process_input() {
    ImGuiIO& io = ImGui::GetIO();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        const glm::ivec2& mouse_window_position {mouse.get_window_position()};
        const uint32_t mouse_state {mouse.get_mouse_state()};

        ImGui_ImplSDL2_ProcessEvent(&event);
        io.MousePos = ImVec2(mouse_window_position.x, mouse_window_position.y);
        io.MouseDown[0] = mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1] = mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT);

        switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                }

                if (event.key.keysym.sym == SDLK_d) {
                    debug_mode = !debug_mode;
                    spdlog::info("Debug mode: ", debug_mode);
                }

                break;
        }
    }
}

void Game::update() {
    // Calculate the amount of time to delay (assuming positive)
    int time_to_delay {
        constants::MILLIS_PER_FRAME - (
            static_cast<int>(SDL_GetTicks()) - millis_previous_frame
        )
    };

    // Check the delay period is positive and delay if so
    if (time_to_delay > 0 && time_to_delay <= constants::MILLIS_PER_FRAME) {
        SDL_Delay(time_to_delay);
    }

    // May be unsused until movement logic is implemented
    [[maybe_unused]] double delta_time {
        (SDL_GetTicks() - millis_previous_frame) / 1'000.f
    };

    // Update the mouse position
    mouse.update(camera->get_position());

    // Update the camera position
    camera->update(display_mode, mouse.get_window_position());

    // To be extracted to its own function call - movement logic
    movement_update(registry, mousemap, delta_time);

    // Update the member to indicate the time the last update was run
    millis_previous_frame = SDL_GetTicks();
}

bool transform_y_comparison(const Transform& lhs, const Transform& rhs) {
    return lhs.position.y < rhs.position.y;
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    const SDL_Rect& camera_position {camera->get_position()};

    registry.sort<Transform>(transform_y_comparison);

    auto terrain_tiles = registry.view<Transform, TerrainSprite>();
    for (auto entity: terrain_tiles) {
        auto& transform {terrain_tiles.get<Transform>(entity)};
        auto& sprite {terrain_tiles.get<TerrainSprite>(entity)};
        render_sprite(renderer, camera_position, render_rect, transform, sprite);
    }

    auto vertical_tiles = registry.view<Transform, VerticalSprite>();
    vertical_tiles.use<Transform>();
    for (auto entity: vertical_tiles) {
        auto& transform {vertical_tiles.get<Transform>(entity)};
        auto& sprite {vertical_tiles.get<VerticalSprite>(entity)};
        render_sprite(renderer, camera_position, render_rect, transform, sprite);
    }

    if (debug_mode) {
        render_imgui_gui(renderer, registry, textures[15], mouse);
        render_bounding_box(registry, camera_position, renderer);
    }

    SDL_RenderPresent(renderer);
}

void Game::run() {
    is_running = true;
    while (is_running) {
        process_input();
        update();
        render();
    }
}

void Game::destroy() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}