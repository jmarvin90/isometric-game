
#include <cmath>
#include <SDL2/SDL_image.h>
#include "spdlog/spdlog.h"

#include "mouse.h"
#include "constants.h"
#include "point.h"

Mouse::Mouse(const std::string mousemap_file_path):
    mousemap {IMG_Load(mousemap_file_path.c_str())} 
{
    if (!mousemap) {
        spdlog::error("Could not load mousemap from " + mousemap_file_path);
    }
    spdlog::info("Mouse constructor called.");
}

Mouse::~Mouse() {
    spdlog::info("Mouse destructor called.");
    SDL_FreeSurface(mousemap);
}

// Query the pixel colour for a pixel on the mousemap at position x, y
SDL_Color Mouse::mousemap_pixel_colour(const Point& pixel_offset) const {
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
Point Mouse::pixel_colour_vector(const SDL_Colour& colour) const {

    if (colour.r == 255 && colour.g == 255 && colour.b == 255) {
        return Point{0, 0};
    }

    if (colour.r == 255) {
        return Point{-1, 0};
    }

    if (colour.g == 255) {
        return Point{0, -1};
    }

    if (colour.b == 255) {
        return Point{1, 0};
    }

    if (colour.r == 0 && colour.g == 0 && colour.b == 0) {
        return Point{0, 1};
    }

    return Point{0, 0};
}

// Calculate the 'coarse' grid position tile walk map
Point Mouse::tile_walk(const Point& tile_offset) const {
    Point vertical{0, 0};
    Point horizontal{0, 0};

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
Point Mouse::pixel_to_grid() const {
    // Coarse coordinates
    int screen_offset_x {position.x - constants::TILEMAP_X_START};
    int screen_offset_y {position.y - constants::TILEMAP_Y_START};

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

    Point coarse {tile_walk({tile_offset_x, tile_offset_y})};

    SDL_Color pixel_colour = mousemap_pixel_colour(
        Point({abs(remainder_x), abs(remainder_y)})
    );

    return coarse + pixel_colour_vector(pixel_colour);
}

void Mouse::update() {
    previous_position = position;
    SDL_GetMouseState(&position.x, &position.y);
    if (position != previous_position) {
        spdlog::info(
            "Mouse position: " + 
            std::to_string(position.x) + ", " + 
            std::to_string(position.y)
        );

        Point grid_position {pixel_to_grid()};
        spdlog::info(
            "Grid position: " + 
            std::to_string(grid_position.x) + ", " +
            std::to_string(grid_position.y)
        );
    }
}