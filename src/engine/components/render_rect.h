#ifndef RENDERRECT_H
#define RENDERRECT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

struct RenderRect {
    SDL_Rect target_rect;

    RenderRect(SDL_Rect target_rect): target_rect{target_rect} {};
    RenderRect(int x, int y, int h, int w): target_rect{x, y, w, y} {};
    ~RenderRect() = default;

    glm::ivec2 bottom_middle() {
        return glm::ivec2(
            target_rect.x + (target_rect.w / 2),
            target_rect.y + target_rect.h
        );
    }
};

#endif