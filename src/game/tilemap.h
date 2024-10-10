#ifndef TILEMAP_H
#define TILEMAP_H

#include <entt/entt.hpp>
#include <vector>

const std::unordered_map<char, glm::vec2> tile_walk_map {
        // Screen tile offset   // Map tile offset
    {   'L',                    glm::vec2(-1, 1)    },
    {   'D',                    glm::vec2(1, 1)     },
    {   'R',                    glm::vec2(1, -1)    },
    {   'U',                    glm::vec2(-1, -1)   }
};

class TileMap {
    SDL_Surface* mousemap;
    std::vector<std::vector<entt::entity>> tilemap;
    SDL_Color mousemap_pixel_colour(const int x, const int y) const;
    glm::vec2 pixel_colour_vector(const SDL_Colour& colour) const;
    glm::vec2 tile_walk(const glm::vec2& tile_screen_pos) const;
    
    public:
        TileMap(entt::registry& registry, const std::string mousemap_file_path);
        ~TileMap();
        entt::entity at(const int x, const int y);
        glm::vec2 pixel_to_grid(const int x, const int y) const;
        glm::vec2 grid_to_pixel(const int x, const int y) const;
};

#endif