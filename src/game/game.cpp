#include "game.h"
#include "spdlog/spdlog.h"
#include <string>
#include <SDL2/SDL_image.h>
#include "../components/transform.h"
#include "../components/sprite.h"

Game::Game() {
    spdlog::info("Game constructor called.");
}

Game::~Game() {
    spdlog::info("Game destuctor called.");
}

void Game::load_tile_textures(){
    std::vector<std::string> tile_paths {
        "./assets/road.png",                // 0
        "./assets/green.png",               // 1
        "./assets/blue.png",                // 2
        "./assets/pink.png",                // 3
        "./assets/tallest_tile.png",        // 4
        "./assets/tall_tile.png",           // 5
        "./assets/taller_tile.png",         // 6
        "./assets/BLBR.png",                // 7
        "./assets/BLTL.png",                // 8
        "./assets/BLTR.png",                // 9
        "./assets/BRTR.png",                // 10
        "./assets/TLBR.png",                // 11
        "./assets/TLTR.png",                // 12
        "./assets/BLBRTR.png"               // 13
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
    const std::vector<std::vector<int>> tile_map {
        {0, 2, 3, 4, 6},
        {4, 7, 9, 10, 3}, 
        {2, 11, 6, 11, 6},
        {1, 8, 13, 12, 3},
        {3, 5, 11, 2, 6}
    };

    const int initial_x {(WINDOW_WIDTH / 2) - (TILE_WIDTH / 2)};
    const int initial_y {100};

    for (int col=0; col<static_cast<int>(tile_map.size()); col++) {
        for (int row=0; row<static_cast<int>(tile_map.at(col).size()); row++) {

            int x_offset {row-col};
            int y_offset {col+row};

            int height_px;
            int width_px;
            int texture_id {tile_map.at(row).at(col)};

            glm::vec2 position{
                initial_x + (x_offset * (TILE_WIDTH / 2)),
                initial_y + (y_offset * (TILE_HEIGHT / 2))
            };

            SDL_QueryTexture(
                textures[texture_id], NULL, NULL, &width_px, &height_px
            );

            int vertical_offset_px {TILE_HEIGHT - height_px};
            int horizontal_offset_px {TILE_WIDTH - width_px};

            auto entity {registry.create()};
            registry.emplace<Transform>(entity, position, 0.0);
            registry.emplace<Sprite>(
                entity,
                height_px,
                width_px,
                vertical_offset_px,
                horizontal_offset_px,
                texture_id
            );
        }
    }
}

void Game::initialise() {
    SDL_Init(SDL_INIT_EVERYTHING);

    // Create the SDL Window
    window = SDL_CreateWindow(
        NULL,                       // title - leave blank for now
        SDL_WINDOWPOS_CENTERED,     // Window x position (centred)
        SDL_WINDOWPOS_CENTERED,     // Window y position (centred)
        WINDOW_WIDTH,               // X res from constant
        WINDOW_HEIGHT,              // Y res from constant
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

    load_tile_textures();
    load_tilemap();

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

void Game::update() {
    // Perform logic to update the game state FIRST, as an experiment
    // ...

    // Calculate the amount of time to delay (assuming positive)
    int time_to_delay {
        MILLIS_PER_FRAME - (
            static_cast<int>(SDL_GetTicks()) - millis_previous_frame
        )
    };

    // May be unsused until movement logic is implemented
    [[maybe_unused]] double delta_time {time_to_delay / 1'000.f};

    // Check the delay period is positive and delay if so
    if (time_to_delay > 0) {
        SDL_Delay(time_to_delay);
    }

    // Update the member to indicate the time the last update was run
    millis_previous_frame = SDL_GetTicks();
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // TODO: implement rendering logic
    // ...

    registry.sort<Transform>([](const Transform& rhs, const Transform& lhs) {
        return rhs.position.y < lhs.position.y;
    });

    auto view {registry.view<Transform, Sprite>()};
    for (auto entity: view) {
        auto& transform {view.get<Transform>(entity)};
        auto& sprite {view.get<Sprite>(entity)};

        // Assume we fetch the whole texture from the top left
        SDL_Rect source_rect{0, 0, sprite.width_px, sprite.height_px};
        
        SDL_Rect dest_rect{
            static_cast<int>(transform.position.x) + sprite.horitonzal_offset_px, 
            static_cast<int>(transform.position.y) + sprite.vertical_offset_px, 
            sprite.width_px,
            sprite.height_px
        };

        SDL_RenderCopyEx(
            renderer,
            textures[sprite.texture_id],
            &source_rect,
            &dest_rect,
            transform.rotation,
            NULL,
            SDL_FLIP_NONE
        );
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