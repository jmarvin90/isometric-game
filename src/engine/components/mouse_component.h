#ifndef MOUSE_H
#define MOUSE_H

#include <glm/glm.hpp>

struct MouseComponent {
    uint32_t mouse_state;
    glm::ivec2 window_current_position;
    glm::ivec2 window_previous_position;
    glm::ivec2 world_position;
    bool moved;
};

#endif