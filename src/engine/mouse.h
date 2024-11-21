#ifndef MOUSE_H
#define MOUSE_H

#include "mousemap.h"

#include <string>
#include "SDL2/SDL.h"
#include <glm/glm.hpp>
#include <imgui.h>

class Mouse {
    uint32_t mouse_state;
    glm::ivec2 window_previous_position;
    glm::ivec2 window_current_position;
    glm::ivec2 world_position;
    glm::ivec2 grid_position;
    const MouseMap& mousemap;

    void set_position(const SDL_Rect& camera);

    public:
        Mouse(const MouseMap& mousemap);
        ~Mouse();

        void update(const SDL_Rect& camera);
        const glm::ivec2& get_window_position() const;
        const glm::ivec2& get_world_position() const;
        const glm::ivec2& get_grid_position() const;
        const uint32_t get_mouse_state() const;
        const bool has_moved_this_frame() const;
        const bool is_on_world_grid() const;
        void update_imgui_io(ImGuiIO& io) const;
};

#endif