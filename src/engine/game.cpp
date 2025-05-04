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

Game::Game() : registry{ entt::registry() },
tilemap{ registry },
mousemap{ constants::MOUSE_MAP_PNG_PATH },
mouse{ mousemap }
{
    spdlog::info("Game constructor called.");
}

Game::~Game()
{
    spdlog::info("Game destructor called.");
}

void Game::load_spritesheets()
{
    spdlog::info("Loading spritesheets");

    city_tiles.emplace(
        constants::MAP_TILE_PNG_PATH, constants::MAP_ATLAS_PATH, renderer->renderer);

    building_tiles.emplace(
        constants::BUILDING_TILE_PNG_PATH, constants::BUILDING_ATLAS_PATH, renderer->renderer);

    vehicle_tiles.emplace(
        constants::VEHICLE_TILE_PNG_PATH, constants::VEHICLE_ATLAS_PATH, renderer->renderer);

    spdlog::info("Sprites loaded");
}

void Game::load_tilemap()
{
    spdlog::info("Loading tilemap");
    for (int y = 0; y < constants::MAP_SIZE_N_TILES; y++)
    {
        for (int x = 0; x < constants::MAP_SIZE_N_TILES; x++)
        {

            glm::ivec2 position{ tilemap[{x, y}].world_position() };

            entt::entity entity{ tilemap[{x, y}].get_entity() };

            registry.emplace<Transform>(entity, position, 0, 0.0);

            // TODO: probably memory issue for uninitialised members
            TileSpriteDefinition sprite_def;

            // TODO: sort this mess out
            if ((x == 8 || x == 7) && y == 0)
            {
                sprite_def = building_tiles->get_sprite_definition("buildingTiles_014.png");
                registry.emplace<Sprite>(
                    entity,
                    building_tiles->get_spritesheet_texture(),
                    sprite_def.texture_rect);
            }
            else if (x == 6 && y == 2)
            {
                sprite_def = building_tiles->get_sprite_definition("buildingTiles_028.png");
                registry.emplace<Sprite>(
                    entity,
                    building_tiles->get_spritesheet_texture(),
                    sprite_def.texture_rect);
            }
            else if (y == 1)
            {
                sprite_def = city_tiles->get_sprite_definition("cityTiles_036.png");
                registry.emplace<Sprite>(
                    entity,
                    city_tiles->get_spritesheet_texture(),
                    sprite_def.texture_rect);
            }
            else
            {
                sprite_def = city_tiles->get_sprite_definition("cityTiles_072.png");
                registry.emplace<Sprite>(
                    entity,
                    city_tiles->get_spritesheet_texture(),
                    sprite_def.texture_rect);
            }

            tilemap[{x, y}].set_connection_bitmask(sprite_def.connection);
        }
    }
}

entt::entity Game::create_entity()
{
    return registry.create();
}

void Game::initialise()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GetDesktopDisplayMode(0, &display_mode);
    camera.emplace(display_mode);

    // Create the SDL Window
    window = SDL_CreateWindow(
        NULL,                   // title - leave blank for now
        SDL_WINDOWPOS_CENTERED, // Window xconstant position (centred)
        SDL_WINDOWPOS_CENTERED, // Window y position (centred)
        display_mode.w,         // X res from current display mode
        display_mode.h,         // Y res from current display mode
        SDL_WINDOW_FULLSCREEN   // Input grabbed flag
    );

    if (!window)
    {
        spdlog::error("Could not initialise SDL Window.");
    }

    renderer.emplace(window, display_mode, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC, -1);

    load_spritesheets();
    load_tilemap();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer->renderer);
    ImGui_ImplSDLRenderer2_Init(renderer->renderer);
}

void Game::process_input()
{

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGuiIO& io = ImGui::GetIO();
        mouse.update_imgui_io(io);
        ImGui_ImplSDL2_ProcessEvent(&event);

        switch (event.type)
        {

        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                is_running = false;
            }

            if (event.key.keysym.sym == SDLK_d)
            {
                debug_mode = !debug_mode;
            }

            break;

        case SDL_MOUSEBUTTONDOWN:
            if (!io.WantCaptureMouse)
            {
                if (tilemap.selected_tile)
                {
                    tilemap.selected_tile = nullptr;
                }
                else
                {
                    if (mouse.is_on_world_grid())
                    {
                        tilemap.selected_tile = &tilemap[mouse.get_grid_position()];
                    }
                }
            }

            break;
        }
    }
}

void Game::update(const float delta_time)
{

    // Update the mouse position
    mouse.update(camera->get_position());

    // Update the camera position
    camera->update(display_mode, mouse.get_window_position());

    // Move relevant entities
    movement_update(registry, mousemap, delta_time);
}

bool transform_comparison(const Transform& lhs, const Transform& rhs)
{
    if (
        lhs.z_index < rhs.z_index ||
        (lhs.z_index == rhs.z_index && lhs.position.y < rhs.position.y))
    {
        return true;
    }
    return false;
}

void Game::render()
{
    const glm::ivec2 camera_position{ camera->get_position() };
    registry.sort<Transform>(transform_comparison);

    renderer->render(registry, camera_position, debug_mode);

    if (debug_mode)
    {
        render_imgui_gui(
            renderer->renderer, registry, mouse, tilemap, city_tiles.value(), building_tiles.value());

        // Highlight tiles if relevant
        // TODO: move this somewhere more appropriate
        if (tilemap.selected_tile || mouse.is_on_world_grid())
        {

            Tile& focus_tile{ (tilemap.selected_tile) ? *tilemap.selected_tile : tilemap[mouse.get_grid_position()] };

            if (tilemap.selected_tile)
            {
                SDL_SetRenderDrawColor(
                    renderer->renderer,
                    0, 0, 255, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(
                    renderer->renderer,
                    255, 0, 0, 255);
            }

            SDL_Point points_to_draw[5];
            focus_tile.get_tile_iso_points(points_to_draw, camera_position);

            SDL_RenderDrawLines(
                renderer->renderer,
                &points_to_draw[0],
                5);
        }
    }

    // TODO: RenderPresent should be called in Renderer::; can't atm because of IMGUI
    SDL_RenderPresent(renderer->renderer);
}

void Game::run()
{
    is_running = true;
    while (is_running)
    {
        // The start point (in ticks), the delta to the last frame in s/ms
        const uint64_t start{ SDL_GetTicks64() };
        const uint64_t since_last_frame{ start - _last_time };
        const float delta_time = { since_last_frame / 1'000.f };

        process_input();
        update(delta_time);
        render();

        // How many millis have elapsed this frame
        const uint64_t elapsed_this_frame{ SDL_GetTicks64() - start };

        // Delay until the START of the next frame
        const float time_to_delay{ constants::MILLIS_PER_FRAME - elapsed_this_frame };

        if (time_to_delay > 0 && time_to_delay <= constants::MILLIS_PER_FRAME)
        {
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