#ifndef BOUNDINGBOXRENDER_H
#define BOUNDINGBOXRENDER_H

#include <entt/entt.hpp>
#include <SDL2/SDL.h>

#include "camera.h"
#include "transform.h"
#include "sprite.h"

void render_bounding_box(const entt::registry& registry, const SDL_Rect& camera, SDL_Renderer* renderer) {
    auto vertical_tiles = registry.view<Transform, VerticalSprite>();
    vertical_tiles.use<Transform>();
    for (auto entity: vertical_tiles) {

        const Transform& transform {vertical_tiles.get<Transform>(entity)};
        const VerticalSprite& sprite {vertical_tiles.get<VerticalSprite>(entity)};

        SDL_Rect bounding_box {
            (static_cast<int>(transform.position.x) + sprite.horizontal_offset_px) - camera.x, 
            (static_cast<int>(transform.position.y) + sprite.vertical_offset_px) - camera.y, 
            sprite.width_px,
            sprite.height_px
        };
                
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }

}

#endif