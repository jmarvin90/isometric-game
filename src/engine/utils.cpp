#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <cmath>

#include "utils.h"
#include "constants.h"

glm::vec2 get_offset(const SDL_Rect& source_rect) {
    return glm::vec2{
        (
            (constants::TILE_SIZE.x - source_rect.w) / 2) + 
            (
                (constants::TILE_SIZE.x - source_rect.w) % 2 != 0
            ),
        constants::TILE_SIZE.y - source_rect.h
    };
}

int direction_index(const uint8_t direction) {
    return __builtin_ctz(direction);
}

glm::ivec2 as_vector(const uint8_t direction) {
    return constants::VECTORS.at(direction_index(direction));
}

uint8_t reverse_direction(const uint8_t direction) {
    return (direction >> 2 | direction << 2) & 15;
}

uint8_t reverse_elevation(const uint8_t elevation) {
    return (elevation >> 2 | elevation << 2) & 240;
}

bool in_bounds(const glm::ivec2 position) {
    bool x_in_bounds{
        0 <= position.x && size_t(position.x) <= constants::MAP_SIZE_N_TILES
    };

    bool y_in_bounds{
        0 <= position.y && size_t(position.y) <= constants::MAP_SIZE_N_TILES
    };

    return x_in_bounds && y_in_bounds;
}

int distance_between(const glm::ivec2 point_a, const glm::ivec2 point_b) {
    int x_diff{ std::abs(point_a.x - point_b.x) };
    int y_diff{ std::abs(point_a.y - point_b.y) };
    return static_cast<int>(std::sqrt(x_diff * x_diff + y_diff * y_diff));
}