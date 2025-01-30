#include "mousemap.h"
#include "constants.h"

#include <SDL2/SDL_image.h>
#include <spdlog/spdlog.h>

MouseMap::MouseMap(const std::string& mousemap_file_path):
    mousemap {IMG_Load(mousemap_file_path.c_str())} 
{
    spdlog::info("MouseMap constructor called.");
    if (!mousemap) {
        spdlog::error("Could not load mousemap from " + mousemap_file_path);
    }
}

MouseMap::~MouseMap() {
    spdlog::info("MouseMap destructor called.");
    SDL_FreeSurface(mousemap);
}

// Query the pixel colour for a pixel on the mousemap at position x, y
SDL_Color MouseMap::mousemap_pixel_colour(const glm::ivec2& pixel_offset) const {
    const unsigned int bpp {mousemap->format->BytesPerPixel};
    
    // TODO: understand wtf happens here
    // Void pointer converted to int pointer and dereferenced?
    unsigned char* pixel {
        (Uint8*)mousemap->pixels + 
        pixel_offset.y * mousemap->pitch + 
        pixel_offset.x * bpp
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
glm::ivec2 MouseMap::pixel_colour_vector(const SDL_Colour& colour) const {

    if (colour.r == 255 && colour.g == 255 && colour.b == 255) {
        return constants::directions[constants::NO_DIRECTION];
    }

    if (colour.r == 255) {
        return constants::directions[constants::LEFT];
    }

    if (colour.g == 255) {
        return constants::directions[constants::UP];
    }

    if (colour.b == 255) {
        return constants::directions[constants::RIGHT];
    }

    if (colour.r == 0 && colour.g == 0 && colour.b == 0) {
        return constants::directions[constants::DOWN];
    }

    return constants::directions[constants::NO_DIRECTION];
}

// Calculate the 'coarse' grid position tile walk map
glm::ivec2 MouseMap::tile_walk(const glm::ivec2& tile_offset) const {
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
glm::ivec2 MouseMap::pixel_to_grid(const glm::ivec2& pixel_coordinate) const {
    // Where we are in relation to the tilemap start (0th tile, top left)
    glm::ivec2 screen_offset{pixel_coordinate - constants::TILEMAP_START};
    
    // How many whole tiles between us and the tilemap start
    glm::ivec2 tile_offset {screen_offset / constants::TILE_SIZE};

    int remainder_x{0};
    int remainder_y{0};
    
    // How many horizontal pixels between us and the nearest whole tile
    if (screen_offset.x < 0) {
        remainder_x = (
            constants::TILE_SIZE.x + 
            (screen_offset.x % constants::TILE_SIZE.x)
        );
    } else {
        remainder_x = screen_offset.x % constants::TILE_SIZE.x;
    }

    // How many vertical pixels between us and the nearest whole tile
    if (screen_offset.y < 0) {
        remainder_y = (
            constants::TILE_SIZE.y +
            (screen_offset.y % constants::TILE_SIZE.y)
        );
    } else {
        remainder_y = screen_offset.y % constants::TILE_SIZE.y;
    }

    // Get a rough grid location based on whole tiles to tilemap start
    glm::ivec2 coarse {tile_walk(tile_offset)};

    // Get an adjustment vector from the mousemap using the remaining pixels
    SDL_Color pixel_colour = mousemap_pixel_colour(
        glm::ivec2{remainder_x, remainder_y}
    );

    // Return the mouse-map adjusted (fine) grid location
    return coarse + pixel_colour_vector(pixel_colour);
}