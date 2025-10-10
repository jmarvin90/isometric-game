#ifndef MOUSEPOSITION_H
#define MOUSEPOSITION_H

#include <tilemap.h>
#include <glm/glm.hpp>


class MousePosition {
    private:
        uint32_t mouse_state;
        const TileMap& tilemap;
        const glm::ivec2& camera_position;
        const int scene_border_px;
        glm::ivec2 m_mouse_window_previous_position;
        glm::ivec2 m_mouse_window_current_position;
        
    public:
        MousePosition(
            const TileMap& tilemap,
            const glm::ivec2& camera_position, 
            const int scene_border_px
        )
        : tilemap {tilemap}
        , camera_position {camera_position}
        , scene_border_px {scene_border_px}
        {}

        void update();
        bool moved() const;
        const glm::ivec2 on_grid() const;
        const glm::ivec2 on_screen() const;
        const glm::ivec2 in_world() const;
};

#endif