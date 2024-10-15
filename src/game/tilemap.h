#ifndef TILEMAP_H
#define TILEMAP_H

#include <entt/entt.hpp>
#include <vector>

#include "point.h"

class TileMap {
    std::vector<std::vector<entt::entity>> tilemap;
    SDL_Color mousemap_pixel_colour(const int x, const int y) const;
    Point pixel_colour_vector(const SDL_Colour& colour) const;
    Point tile_walk(const Point& tile_screen_pos) const;
    
    public:
        TileMap(entt::registry& registry);
        ~TileMap();
        entt::entity at(const int x, const int y);
        Point pixel_to_grid(const int x, const int y) const;
        Point grid_to_pixel(const int x, const int y) const;
};

#endif