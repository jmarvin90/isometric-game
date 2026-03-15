#ifndef UTILITY_H
#define UTILITY_H

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace Utility {

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
};

enum class SpriteAnchor {
    SPRITE_ANCHOR = 1,
    ORIGIN = 2
};

void align_sprite_to(
    entt::registry& registry,
    entt::entity entity,
    SpriteAnchor alignment_anchor,
    glm::ivec2 position
);
} // namespace

#endif