#include <cmath>

#include "SDL2/SDL_image.h"
#include "SDL2/SDL.h"
#include "spdlog/spdlog.h"
#include "glm/glm.hpp"

#include "tilemap.h"
#include "constants.h"


// Create the vector of tile entities and load the mousemap surface.
TileMap::TileMap(
    entt::registry& registry,
    const std::string mousemap_file_path
):mousemap{IMG_Load(mousemap_file_path.c_str())} 
{
    spdlog::info("TileMap constructor called.");

    // Create the entities associated with the map
    for (int x=0; x<constants::MAP_SIZE; x++) {
        std::vector<entt::entity> row;
        for (int y=0; y<constants::MAP_SIZE; y++) {
            row.push_back(registry.create());
        }
        tilemap.push_back(row);
    }
}

// Ensure to free the mousemap surface. Question re. destruction of entities
TileMap::~TileMap() {
    spdlog::info("TileMap destructor called.");
    SDL_FreeSurface(mousemap);
}

// Get an entity from tilemap position x, y
entt::entity TileMap::at(const int x, const int y) {
    return tilemap.at(x).at(y);
}

// Query the pixel colour for a pixel on the mousemap at position x, y
SDL_Color TileMap::mousemap_pixel_colour(const int x, const int y) const {
    const unsigned int bpp {mousemap->format->BytesPerPixel};
    
    // TODO: understand wtf happens here
    // Void pointer converted to int pointer and dereferenced?
    unsigned char* pixel {(Uint8*)mousemap->pixels + y * mousemap->pitch + x * bpp};
    unsigned int pixel_data = *(unsigned int*)pixel;

    SDL_Color pixel_colour {0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE};

    SDL_GetRGB(
        pixel_data,
        mousemap->format,
        &pixel_colour.r,
        &pixel_colour.g,
        &pixel_colour.b
    );

    return pixel_colour;
}

// Convert the pixel colour into a vector to be added to a 'coarse' grid location
glm::vec2 TileMap::pixel_colour_vector(const SDL_Colour& colour) const {

    if (colour.r == 255 && colour.g == 255 && colour.b == 255) {
        return glm::vec2(0, 0);
    }

    if (colour.r == 255) {
        return glm::vec2(-1, 0);
    }

    if (colour.g == 255) {
        return glm::vec2(0, -1);
    }

    if (colour.b == 255) {
        return glm::vec2(1, 0);
    }

    if (colour.r == 0 && colour.g == 0 && colour.b == 0) {
        return glm::vec2(0, 1);
    }

    return glm::vec2(0, 0);
}

// Calculate the 'coarse' grid position tile walk map
glm::vec2 TileMap::tile_walk(const glm::vec2& tile_screen_pos) const {
    glm::vec2 vertical{0, 0};
    glm::vec2 horizontal{0, 0};

    if (tile_screen_pos.x != 0) {
        horizontal = horizontal_vector * tile_screen_pos.x;
    }

    // If we're above origin (we shouldn't be?)
    if (tile_screen_pos.y != 0) {
        vertical = vertical_vector * tile_screen_pos.y;
    }

    return vertical + horizontal;
}

// Public function converting x, y screen coordinates into tilemap coordinates
glm::vec2 TileMap::pixel_to_grid(const int x, const int y) const {
    // Coarse coordinates
    int screen_offset_x {x - constants::TILEMAP_X_START};
    int screen_offset_y {y - constants::TILEMAP_Y_START};

    spdlog::info(
        "Screen offset: " + 
        std::to_string(screen_offset_x) + ", " + 
        std::to_string(screen_offset_y)
    );

    double tile_offset_x {floor(screen_offset_x / static_cast<double>(constants::TILE_WIDTH))};
    double tile_offset_y {floor(screen_offset_y / static_cast<double>(constants::TILE_HEIGHT))};

    spdlog::info(
        "Tile offset: " + 
        std::to_string(tile_offset_x) + ", " +
        std::to_string(tile_offset_y)
    );

    int remainder_x{0};
    int remainder_y{0};
    
    if (screen_offset_x < 0) {
        remainder_x = (
            constants::TILE_WIDTH + 
            (screen_offset_x % constants::TILE_WIDTH)
        );
    } else {
        remainder_x = screen_offset_x % constants::TILE_WIDTH;
    }

    if (screen_offset_y < 0) {
        remainder_y = (
            constants::TILE_HEIGHT +
            (screen_offset_y % constants::TILE_HEIGHT)
        );
    } else {
        remainder_y = screen_offset_y % constants::TILE_HEIGHT;
    }

    spdlog::info(
        "Remainder: " + 
        std::to_string(remainder_x) + ", " +
        std::to_string(remainder_y)
    );

    glm::vec2 coarse {tile_walk(glm::vec2(tile_offset_x, tile_offset_y))};
    spdlog::info(
        "Coarse: " + 
        std::to_string(coarse.x) + ", " +
        std::to_string(coarse.y)
    );

    SDL_Color pixel_colour = mousemap_pixel_colour(abs(remainder_x), abs(remainder_y));
    spdlog::info(
        "Pixel colour: r:" +
        std::to_string(pixel_colour.r) + ", g:" +
        std::to_string(pixel_colour.g) + ", b:" +
        std::to_string(pixel_colour.b)
    );

    glm::vec2 add_vec {pixel_colour_vector(pixel_colour)};
    spdlog::info(
        "Mousemap offset vector: " + 
        std::to_string(add_vec.x) + ", " +
        std::to_string(add_vec.y)
    );

    return coarse + pixel_colour_vector(pixel_colour);
}

// Public function converting x, y tilemap coordinates to screen coordinates
glm::vec2 TileMap::grid_to_pixel(const int x, const int y) const {
    int x_offset {x-y};
    int y_offset {y+x};
    return glm::vec2 {
        constants::TILEMAP_X_START + (x_offset * constants::TILE_WIDTH_HALF),
        constants::TILEMAP_Y_START + (y_offset * constants::TILE_HEIGHT_HALF)
    };
}