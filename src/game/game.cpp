#include <string>

#include <glm/glm.hpp>
#include <SDL2/SDL_image.h>
#include <entt/entt.hpp>
#include "spdlog/spdlog.h"

#include "constants.h"
#include "game.h"

#include "../components/transform.h"
#include "../components/sprite.h"
#include "../components/rigid_body.h"


Game::Game(): 
    registry{entt::registry()}, 
    tilemap{registry},
    mouse{"./assets/mousemap.png"}
{
    spdlog::info("Game constructor called.");
}

Game::~Game() {
    spdlog::info("Game destuctor called.");
}

void Game::load_textures(){
    std::vector<std::string> tile_paths {
        "./assets/road.png",                        // 0
        "./assets/green.png",                       // 1
        "./assets/blue.png",                        // 2
        "./assets/pink.png",                        // 3
        "./assets/tallest_tile.png",                // 4
        "./assets/tall_tile.png",                   // 5
        "./assets/taller_tile.png",                 // 6
        "./assets/BLBR.png",                        // 7
        "./assets/BLTL.png",                        // 8
        "./assets/BLTR.png",                        // 9
        "./assets/BRTR.png",                        // 10
        "./assets/TLBR.png",                        // 11
        "./assets/TLTR.png",                        // 12
        "./assets/BLBRTR.png",                      // 13
        "./assets/moveable_sprite_test.png",        // 14
        "./assets/moveable_sprite_tall_test.png",   // 15
    };

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

            int height_px;
            int width_px;

            SDL_QueryTexture(
                textures[texture_id], NULL, NULL, &width_px, &height_px
            );

            int vertical_offset_px {constants::TILE_HEIGHT - height_px};
            int horizontal_offset_px {constants::TILE_WIDTH - width_px};

            entt::entity entity {tilemap.at(x, y)};
            
            registry.emplace<Transform>(entity, position, 0.0);
            registry.emplace<TerrainSprite>(
                entity,
                height_px,
                width_px,
                texture_id,
                vertical_offset_px,
                horizontal_offset_px
            );
        }
    }

    for (int n=0; n<3; n++) {
        entt::entity entity_1 {tilemap.at(n, 3)};
        entt::entity entity_2 {tilemap.at(n, 1)};

        int height_px;
        int width_px;

        SDL_QueryTexture(
            textures[n+4], NULL, NULL, &width_px, &height_px
        );

        registry.emplace<VerticalSprite>(
            entity_1,
            height_px,
            width_px,
            n+4,
            constants::TILE_HEIGHT - height_px,
            constants::TILE_WIDTH - width_px
        );

        registry.emplace<VerticalSprite>(
            entity_2,
            height_px,
            width_px,
            n+4,
            constants::TILE_HEIGHT - height_px,
            constants::TILE_WIDTH - width_px
        );
    }
}

void Game::initialise() {
    SDL_Init(SDL_INIT_EVERYTHING);

    // Create the SDL Window
    window = SDL_CreateWindow(
        NULL,                       // title - leave blank for now
        SDL_WINDOWPOS_CENTERED,     // Window x position (centred)
        SDL_WINDOWPOS_CENTERED,     // Window y position (centred)
        constants::WINDOW_WIDTH,    // X res from constant
        constants::WINDOW_HEIGHT,   // Y res from constant
        SDL_WINDOW_INPUT_GRABBED    // Input grabbed flag
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

    load_textures();
    load_tilemap();

    int height_px;
    int width_px;

    SDL_QueryTexture(
        textures[15], NULL, NULL, &width_px, &height_px
    );

    SDL_RenderSetClipRect(renderer, &render_rect);

    // TODO: remove
    entt::entity entity {registry.create()};
    glm::vec2 position {tilemap.grid_to_pixel(5, 2)};
    position.x += (constants::TILE_WIDTH_HALF - (width_px / 2));
    position.y -= (constants::TILE_HEIGHT_HALF);
    registry.emplace<Transform>(entity, position, 0.0);
    registry.emplace<VerticalSprite>(entity, height_px, width_px, 15, constants::TILE_HEIGHT-height_px, 0);
    registry.emplace<RigidBody>(entity, glm::vec2(-60, -30));
    
    // TODO: initialise ImGui
}

void Game::process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                }
                break;
        }
    }
}

void apply_velocity(
    entt::view<entt::get_t<RigidBody, Transform>> moveable_entities, 
    double delta_time
) {
    for (entt::entity entity: moveable_entities) {
        RigidBody& rigid_body = moveable_entities.get<RigidBody>(entity);
        Transform& transform = moveable_entities.get<Transform>(entity);
        transform.position.x += (rigid_body.velocity.x * delta_time);
        transform.position.y += (rigid_body.velocity.y * delta_time);
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
    
    // To be extracted to its own function call
    // movement logic
    apply_velocity(registry.view<RigidBody, Transform>(), delta_time);

    // Update the mouse position
    mouse.set_position(camera.get_position());

    // Update the camera position
    camera.set_position(mouse.get_window_position());

    // Update the member to indicate the time the last update was run
    millis_previous_frame = SDL_GetTicks();
}

int transform_abspixel(const Transform& transform) {
    return (transform.position.y * constants::WINDOW_WIDTH) + transform.position.x;
}

bool transform_y_comparison(const Transform& lhs, const Transform& rhs) {
    int lhs_abspixel {transform_abspixel(lhs)};
    int rhs_abspixel {transform_abspixel(rhs)};
    return lhs_abspixel < rhs_abspixel;
}

void render_sprite(
    SDL_Renderer* renderer,
    const SDL_Rect& camera,
    const std::unordered_map<int, SDL_Texture*>& textures,
    const Transform& transform, 
    const Sprite& sprite
) {

    SDL_Rect source_rect {0, 0, sprite.width_px, sprite.height_px};
    SDL_Rect dest_rect {
        (static_cast<int>(transform.position.x) + sprite.horitonzal_offset_px) - camera.x, 
        (static_cast<int>(transform.position.y) + sprite.vertical_offset_px) - camera.y, 
        sprite.width_px,
        sprite.height_px
    };

    SDL_Texture* texture {textures.find(sprite.texture_id)->second};

    SDL_RenderCopyEx(
        renderer,
        texture,
        &source_rect,
        &dest_rect,
        transform.rotation,
        NULL,
        SDL_FLIP_NONE
    );
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    const SDL_Rect& camera_position {camera.get_position()};

    registry.sort<Transform>(transform_y_comparison);

    auto terrain_tiles = registry.view<Transform, TerrainSprite>();
    for (auto entity: terrain_tiles) {
        auto& transform {terrain_tiles.get<Transform>(entity)};
        auto& sprite {terrain_tiles.get<TerrainSprite>(entity)};
        render_sprite(renderer, camera_position, textures, transform, sprite);
    }

    auto vertical_tiles = registry.view<Transform, VerticalSprite>();
    vertical_tiles.use<Transform>();
    for (auto entity: vertical_tiles) {
        auto& transform {vertical_tiles.get<Transform>(entity)};
        auto& sprite {vertical_tiles.get<VerticalSprite>(entity)};
        render_sprite(renderer, camera_position, textures, transform, sprite);
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
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}