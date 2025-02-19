#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <utility>

#include <glm/glm.hpp>
#include <SDL2/SDL_image.h>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

#include "constants.h"
#include "game.h"
#include "spritesheet.h"

#include "components/transform.h"
#include "components/sprite.h"
#include "components/rigid_body.h"

#include "systems/movement.h"
#include "systems/render.h"
#include "systems/imgui_render.h"

Game::Game(): 
    registry{entt::registry()}, 
    tilemap{registry},
    mousemap{"/home/marv/Documents/Projects/isometric-game/assets/mousemap.png"},
    mouse{mousemap}
{
    spdlog::info("Game constructor called.");
}

Game::~Game() {
    spdlog::info("Game destuctor called.");
}

void Game::load_spritesheets() {
    spdlog::info("Loading spritesheets");

    city_tiles.emplace(
        constants::MAP_TILE_PNG_PATH, constants::MAP_ATLAS_PATH, renderer
    );

    building_tiles.emplace(
        constants::BUILDING_TILE_PNG_PATH, constants::BUILDING_ATLAS_PATH, renderer
    );

    spdlog::info("Sprites loaded");
}

void Game::load_tilemap() {
    spdlog::info("Loading tilemap");
    for (int y=0; y<constants::MAP_SIZE_N_TILES; y++) {
        for (int x=0; x<constants::MAP_SIZE_N_TILES; x++) {

            glm::ivec2 position {tilemap.grid_to_pixel({x, y})};

            entt::entity entity {tilemap.at(x, y)};
            
            registry.emplace<Transform>(entity, position, 0, 0.0);

            std::string tilepng;

            if (x == 8 && y == 0) {
                registry.emplace<Sprite>(
                    entity, 
                    building_tiles->get_spritesheet_texture(),
                    building_tiles->get_sprite_rect("buildingTiles_014.png")
                );
            } else if (y==1) {
                registry.emplace<Sprite>(
                    entity, 
                    city_tiles->get_spritesheet_texture(),
                    city_tiles->get_sprite_rect("cityTiles_036.png")
                );
            } else {
                registry.emplace<Sprite>(
                    entity, 
                    city_tiles->get_spritesheet_texture(),
                    city_tiles->get_sprite_rect("cityTiles_072.png")
                );
            }
        }
    }

    entt::entity building_level {registry.create()};
    glm::ivec2 building_level_position {tilemap.grid_to_pixel({8, 0})};
    building_level_position -= glm::ivec2{0, constants::GROUND_FLOOR_BUILDING_OFFSET};
    registry.emplace<Transform>(
        building_level, building_level_position, 1, 0.0
    );

    registry.emplace<Sprite>(
        building_level, 
        building_tiles->get_spritesheet_texture(),
        building_tiles->get_sprite_rect("buildingTiles_043.png")
    );

    entt::entity second_building_level {registry.create()};
    glm::ivec2 second_building_level_position {
        building_level_position -= glm::ivec2{0, constants::MAX_TILE_DEPTH}
    };
    registry.emplace<Transform>(
        second_building_level, second_building_level_position, 2, 0.0
    );

    registry.emplace<Sprite>(
        second_building_level, 
        building_tiles->get_spritesheet_texture(),
        building_tiles->get_sprite_rect("buildingTiles_043.png")
    );

    entt::entity final_building_level {registry.create()};
    glm::ivec2 final_building_level_position {
        second_building_level_position -= glm::ivec2{0, constants::MAX_TILE_DEPTH}
    };
    registry.emplace<Transform>(
        final_building_level, final_building_level_position, 3, 0.0
    );

    registry.emplace<Sprite>(
        final_building_level, 
        building_tiles->get_spritesheet_texture(),
        building_tiles->get_sprite_rect("buildingTiles_058.png")
    );
}

entt::entity Game::create_entity() {
    return registry.create();
}

void Game::initialise() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GetDesktopDisplayMode(0, &display_mode);
    camera.emplace(display_mode);

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

    load_spritesheets();
    load_tilemap();

    render_rect = {20, 20, display_mode.w - 40, display_mode.h - 40};

    SDL_RenderSetClipRect(renderer, &render_rect);    

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
}

void Game::process_input() {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        mouse.update_imgui_io(ImGui::GetIO());
        ImGui_ImplSDL2_ProcessEvent(&event);

        switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                }

                if (event.key.keysym.sym == SDLK_d) {
                    debug_mode = !debug_mode;
                }

                break;
        }
    }
}

void Game::update(const float delta_time) {

    // Update the mouse position
    mouse.update(camera->get_position());

    // Update the camera position
    camera->update(display_mode, mouse.get_window_position());

    // Move relevant entities
    movement_update(registry, mousemap, delta_time);
}

bool transform_comparison(const Transform& lhs, const Transform& rhs) {
    if (
        lhs.z_index < rhs.z_index ||
        (lhs.z_index == rhs.z_index && lhs.position.y < rhs.position.y)
    ) {
        return true;
    }
    return false;
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    const glm::ivec2 camera_position {camera->get_position()};

    registry.sort<Transform>(transform_comparison);

    render_sprites(registry, camera_position, renderer, render_rect, debug_mode);

    // TODO: Update
    if (debug_mode) {
        render_imgui_gui(renderer, registry, mouse);

        if (mouse.is_on_world_grid()) {

            glm::ivec2 start_point {
                tilemap.grid_to_pixel(mouse.get_grid_position()) - 
                camera_position
            };

            start_point.y -= constants::MIN_TILE_DEPTH;
        
            SDL_SetRenderDrawColor(
                renderer,
                255, 0, 0, 255
            ); 

            SDL_Point points_to_draw[5] {};

            for (int i=0; i<5; i++) {
                glm::ivec2 point {
                    (constants::TILE_EDGE_POINTS[i] + start_point)
                    //  + glm::ivec2{0, constants::MIN_TILE_DEPTH} 
                };
                points_to_draw[i] = SDL_Point{point.x, point.y}; 
            }
        
            SDL_RenderDrawLines(
                renderer,
                &points_to_draw[0],
                5
                // sizeof(my_points)
            );
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::run() {
    is_running = true;
    while (is_running) {
        // The start point (in ticks), the delta to the last frame in s/ms 
        const uint64_t start {SDL_GetTicks64()};
        const uint64_t since_last_frame {start - _last_time};
        const float delta_time = {since_last_frame / 1'000.f};

        process_input();
        update(delta_time);
        render();

        // How many millis have elapsed this frame
        const uint64_t elapsed_this_frame {SDL_GetTicks64() - start};

        // Delay until the START of the next frame
        const float time_to_delay {constants::MILLIS_PER_FRAME - elapsed_this_frame};

        if (time_to_delay > 0 && time_to_delay <= constants::MILLIS_PER_FRAME) {
            SDL_Delay(time_to_delay);
        }

        _last_time = start;
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