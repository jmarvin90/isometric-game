#ifndef SPATIALMAPCELLCOMPONENT_H
#define SPATIALMAPCELLCOMPONENT_H

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <vector>

struct SpatialMapCellComponent {
    SDL_Rect cell;
    std::vector<entt::entity> entities;
    std::vector<entt::entity> segments;
};

#endif