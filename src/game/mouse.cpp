
#include <cmath>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <glm/ext/vector_int1.hpp>
#include "spdlog/spdlog.h"

#include "mouse.h"
#include "constants.h"

Mouse::Mouse(const std::string mousemap_file_path):
    mousemap {IMG_Load(mousemap_file_path.c_str())} 
{
    spdlog::info("Mouse constructor called.");
    if (!mousemap) {
        spdlog::error("Could not load mousemap from " + mousemap_file_path);
    }
}

Mouse::~Mouse() {
    spdlog::info("Mouse destructor called.");
    SDL_FreeSurface(mousemap);
}

// Query the pixel colour for a pixel on the mousemap at position x, y
SDL_Color Mouse::mousemap_pixel_colour(const glm::ivec2& pixel_offset) const {
    const unsigned int bpp {mousemap->format->BytesPerPixel};
    
    // TODO: understand wtf happens here
    // Void pointer converted to int pointer and dereferenced?
    unsigned char* pixel {
        (Uint8*)mousemap->pixels + 
        static_cast<int>(pixel_offset.y) * mousemap->pitch + 
        static_cast<int>(pixel_offset.x) * bpp
    };

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
glm::ivec2 Mouse::pixel_colour_vector(const SDL_Colour& colour) const {

    if (colour.r == 255 && colour.g == 255 && colour.b == 255) {
        return glm::ivec2{0, 0};
    }

    if (colour.r == 255) {
        return glm::ivec2{-1, 0};
    }

    if (colour.g == 255) {
        return glm::ivec2{0, -1};
    }

    if (colour.b == 255) {
        return glm::ivec2{1, 0};
    }

    if (colour.r == 0 && colour.g == 0 && colour.b == 0) {
        return glm::ivec2{0, 1};
    }

    return glm::ivec2{0, 0};
}

// Calculate the 'coarse' grid position tile walk map
glm::ivec2 Mouse::tile_walk(const glm::ivec2& tile_offset) const {
    glm::ivec2 vertical{0, 0};
    glm::ivec2 horizontal{0, 0};

    if (tile_offset.x != 0) {
        horizontal = horizontal_vector * tile_offset.x;
    }

    // If we're above origin (we shouldn't be?)
    if (tile_offset.y != 0) {
        vertical = vertical_vector * tile_offset.y;
    }

    return vertical + horizontal;
}

// Public function converting x, y screen coordinates into tilemap coordinates
glm::ivec2 Mouse::pixel_to_grid() const {
    // Coarse coordinates
    int screen_offset_x {static_cast<int>(position.x) - constants::TILEMAP_X_START};
    int screen_offset_y {static_cast<int>(position.y) - constants::TILEMAP_Y_START};

    int tile_offset_x {
        static_cast<int>(
            floor(
                screen_offset_x / static_cast<double>(constants::TILE_WIDTH)
            )
        )
    };

    int tile_offset_y {
        static_cast<int>(
            floor(
                screen_offset_y / static_cast<double>(constants::TILE_HEIGHT)
            )
        )
    };

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

    glm::ivec2 coarse {tile_walk({tile_offset_x, tile_offset_y})};

    SDL_Color pixel_colour = mousemap_pixel_colour(
        glm::ivec2{remainder_x, remainder_y}
    );

    return coarse + pixel_colour_vector(pixel_colour);
}

void Mouse::update() {
    previous_position = position;

    int x, y;
    SDL_GetMouseState(&x, &y);
    position.x = x;
    position.y = y;

    glm::ivec2 grid_position {pixel_to_grid()};

    if (position != previous_position) {
        spdlog::info(
            "Mouse position: " + 
            std::to_string(position.x) + ", " + 
            std::to_string(position.y) + " (" +
            std::to_string(grid_position.x) + ", " +
            std::to_string(grid_position.y) + ")"
        );
    }
}