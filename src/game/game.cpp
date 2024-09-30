#include "game.h"
#include "spdlog/spdlog.h"
#include <SDL2/SDL_image.h>
#include <random>

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

    tile_paths = {
        "./assets/blue.png", 
        "./assets/green.png",
        "./assets/pink.png",
        "./assets/road.png", 
        "./assets/tall_tile.png", 
        "./assets/taller_tile.png",
        "./assets/tallest_tile.png"
    };

    get_textures();
    create_random_tilemap();

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

void Game::create_random_tilemap() {
    spdlog::info("Creating a random tilemap...");

    std::random_device seed;
    std::mt19937 gen{seed()};
    std::uniform_int_distribution<> dist{0, static_cast<int>(tile_paths.size()-1)};

    for (int n=0; n<TILEMAP_SIZE; n++) {
        std::vector<int> tiles;
        for (int m=0; m<TILEMAP_SIZE; m++) {
            tiles.push_back(dist(gen));
        }
        tile_map.push_back(tiles);
    }
}

void Game::get_textures() {
    spdlog::info("Loading textures...");

    for (int n=0; n<static_cast<int>(tile_paths.size()); n++) {
        spdlog::info(tile_paths.at(n));
        SDL_Surface* surface {IMG_Load(tile_paths.at(n).c_str())};
        if (!surface) {
            spdlog::error("Error in loading texture: " + tile_paths.at(n));
        }

        SDL_Texture* texture {SDL_CreateTextureFromSurface(renderer, surface)};
        if (!texture) {
            spdlog::error(
                "Error in creating texture from surface for: " + 
                tile_paths.at(n)
            );
        }

        SDL_FreeSurface(surface);

        textures.emplace(
            std::make_pair(n, texture)
        );
    }
}

void render_tilemap(
    std::unordered_map<int, SDL_Texture*>& textures,
    std::vector<std::vector<int>>& tile_map,
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

    render_tilemap(
        textures,
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