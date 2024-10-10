#include <unordered_map>
#include <cmath>
#include <SDL2/SDL.h>
#include "glm/glm.hpp"
#include "constants.h"


const std::unordered_map<char, glm::vec2> tile_walk_map {
        // Screen tile offset   // Map tile offset
    {   'L',                    glm::vec2(-1, 1)    },
    {   'D',                    glm::vec2(1, 1)     },
    {   'R',                    glm::vec2(1, -1)    },
    {   'U',                    glm::vec2(-1, -1)   }
};

glm::vec2 operator*(glm::vec2 input_vector, const int multiplier) {
    return glm::vec2{
        input_vector.x *= multiplier,
        input_vector.y *= multiplier
    };
}

glm::vec2 tile_walk(const glm::vec2& tile_screen_pos) {
    glm::vec2 vertical{0, 0};
    glm::vec2 horizontal{0, 0};

    // If we're left of origin
    if (tile_screen_pos.x < 0) {
        horizontal = tile_walk_map.at('L') * tile_screen_pos.x;
    }

    // If we're right of origin
    if (tile_screen_pos.x > 0) {
        horizontal = tile_walk_map.at('R') * tile_screen_pos.x;
    }

    // If we're above origin (we shouldn't be?)
    if (tile_screen_pos.y < 0) {
        vertical = tile_walk_map.at('U') * tile_screen_pos.y;
    }

    // If we're below origin
    if (tile_screen_pos.y > 0) {
        vertical = tile_walk_map.at('D') * tile_screen_pos.y;
    }

    return vertical + horizontal;
}

glm::vec2 pixels_to_grid_pos(int x, int y) {
    // Coarse coordinates
    int screen_offset_x {x - constants::TILEMAP_X_START};
    int screen_offset_y {y - constants::TILEMAP_Y_START};
    int tile_offset_x {screen_offset_x / constants::TILE_WIDTH};
    int tile_offset_y {screen_offset_y / constants::TILE_HEIGHT};

    // int remainder_x {screen_offset_x % constants::TILE_WIDTH};
    // int remainder_y {screen_offset_y % constants::TILE_HEIGHT};
    return tile_walk(glm::vec2(tile_offset_x, tile_offset_y));
}

glm::vec2 grid_pos_to_pixels(const int x, const int y) {
    int x_offset {x-y};
    int y_offset {y+x};
    return glm::vec2 {
        constants::TILEMAP_X_START + (x_offset * constants::TILE_WIDTH_HALF),
        constants::TILEMAP_Y_START + (y_offset * constants::TILE_HEIGHT_HALF)
    };
}

glm::vec2 mousemap_colour_to_vector(const SDL_Color& colour) {
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