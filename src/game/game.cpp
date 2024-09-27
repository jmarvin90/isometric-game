#include "game.h"
#include "spdlog/spdlog.h"
#include <unordered_map>
#include <vector>
#include <SDL2/SDL_image.h>

Game::Game() {
    spdlog::info("Game constructor called.");
}

Game::~Game() {
    spdlog::info("Game destuctor called.");
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

std::unordered_map<std::string, SDL_Texture*> get_textures(SDL_Renderer* renderer) {
    std::unordered_map<std::string, SDL_Texture*> textures;

    std::vector<std::string> tile_paths {
        "./assets/blue.png", 
        "./assets/green.png",
        "./assets/pink.png",
        "./assets/road.png", 
        "./assets/tall_tile.png"
    };

    for (std::string tile_path: tile_paths) {
        SDL_Surface* surface {IMG_Load(tile_path.c_str())};
        if (!surface) {
            spdlog::error("Error in loading texture: " + tile_path);
        }

        SDL_Texture* texture {SDL_CreateTextureFromSurface(renderer, surface)};
        if (!texture) {
            spdlog::error("Error in creating texture from surface for: " + tile_path);
        }

        SDL_FreeSurface(surface);

        textures.emplace(
            std::make_pair(tile_path, texture)
        );
    }

    return textures;
}

void render_tilemap(
    std::unordered_map<std::string, SDL_Texture*> textures,
    std::vector<std::vector<std::string>> tile_map,
    SDL_Renderer* renderer
) {
    int initial_x_position {(WINDOW_WIDTH / 2) - (TILE_WIDTH / 2)};
    int initial_y_position {100};

    for (int x = 0; x < static_cast<int>(tile_map.size()); x++) {
        for (int y = 0; y < static_cast<int>(tile_map.at(x).size()); y++) {

            int x_position {initial_x_position + ((y - x) * (TILE_WIDTH / 2))};
            int y_position {initial_y_position + ((y + x) * (TILE_HEIGHT / 2))};

            int height_px;
            int width_px;

            SDL_QueryTexture(
                textures[tile_map.at(y).at(x)],
                NULL,
                NULL,
                &width_px,
                &height_px
            );

            // TODO: interesting offset problem
            // The vertical offset assumes the texture starts at the bottom of the image
            int vertical_offset {(TILE_HEIGHT - height_px)};

            // The horizontal offset assumes the texture starts on the left
            // More than likely it'll be in the middle (as will the vertical)
            int horizontal_offset {(TILE_WIDTH - width_px) / 2};

            [[maybe_unused]] SDL_Rect source_rect {0, 0, 120, 60};
            SDL_Rect dest_rect {
                x_position + horizontal_offset,
                y_position + vertical_offset,
                width_px, 
                height_px
            };

            SDL_RenderCopyEx(
                renderer,
                textures[tile_map.at(y).at(x)],
                // &source_rect,
                NULL,
                &dest_rect,
                0.0,
                NULL,
                SDL_FLIP_NONE
            );
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // TODO: implement rendering logic
    // ...
    std::vector<std::vector<std::string>> tile_map {
        {
            "./assets/blue.png", 
            "./assets/pink.png",
            "./assets/green.png",
            "./assets/road.png"
        },
        {
            "./assets/green.png",
            // "./assets/blue.png", 
            "./assets/tall_tile.png",
            // "./assets/pink.png",
            "./assets/tall_tile.png",
            "./assets/road.png"
        },
        {
            "./assets/pink.png",
            "./assets/green.png",
            // "./assets/blue.png",
            "./assets/tall_tile.png",
            "./assets/road.png"
        },
        {
            "./assets/blue.png", 
            "./assets/green.png",
            "./assets/pink.png",
            "./assets/road.png"
        }
    };

    render_tilemap(
        get_textures(renderer),
        tile_map,
        renderer
    );

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