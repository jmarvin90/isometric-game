#ifndef ISOUTILITY_H
#define ISOUTILITY_H

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include <components/sprite_component.h>
#include <components/transform_component.h>

namespace ISOUtility {

struct SDLDestroyer {
    void operator()(SDL_Window* window) const
    {
        SDL_DestroyWindow(window);
    }
    void operator()(SDL_Renderer* renderer) const
    {
        SDL_DestroyRenderer(renderer);
    }
    void operator()(SDL_Texture* texture) const
    {
        SDL_DestroyTexture(texture);
    }
    void operator()(SDL_Surface* surface) const
    {
        SDL_FreeSurface(surface);
    }
};

bool AABB(
    const TransformComponent& transform,
    const SpriteComponent& sprite,
    const glm::ivec2 position
);

void align_sprite_to(
    entt::registry& registry,
    entt::entity entity,
    TransformComponent& transform,
    const SpriteComponent& sprite,
    SpriteAnchor alignment_anchor,
    glm::ivec2 world_position
);
} // namespace

#endif