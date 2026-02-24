#ifndef POSITION_H
#define POSITION_H

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace Position {

glm::ivec2 screen_to_world(
    const glm::ivec2 screen_position,
    const glm::ivec2 camera_position
);

glm::ivec2 world_to_screen(
    const glm::ivec2 world_position,
    const glm::ivec2 camera_position
);

}

#endif