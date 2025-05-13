#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "constants.h"
#include "tilemap.h"

// glm::vec2 get_offset(const SDL_Rect& source_rect);
int direction_index(const uint8_t direction);
glm::ivec2 as_vector(const uint8_t direction);
uint8_t reverse_direction(const uint8_t direction);
uint8_t reverse_elevation(const uint8_t elevation);
bool in_bounds(const glm::ivec2 position);
int distance_between(const glm::ivec2 point_a, const glm::ivec2 point_b);

#endif