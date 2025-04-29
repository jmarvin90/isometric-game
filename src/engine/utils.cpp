#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "utils.h"
#include "constants.h"

glm::vec2 get_offset(const SDL_Rect& source_rect) {
    return glm::vec2{
        ((constants::TILE_SIZE.x - source_rect.w) / 2)
        + ((constants::TILE_SIZE.x - source_rect.w) % 2 != 0),
        constants::TILE_SIZE.y - source_rect.h    
    };
}

int direction_index(const uint8_t direction) {
    return __builtin_ctz(direction);
}

glm::ivec2 as_vector(const uint8_t direction) {
    return constants::VECTORS.at(direction_index(direction));
}

uint8_t reverse(const uint8_t direction) {
    return (direction >> 2 | direction << 2) & 15;
}