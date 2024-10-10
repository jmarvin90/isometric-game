#ifndef TILEMAP_H
#define TILEMAP_H

#include <entt/entt.hpp>
#include <vector>

class MouseMap {
    SDL_Surface* mousemap;
    public:
        MouseMap(const std::string mousemap_file_path);
        ~MouseMap();
        SDL_Color get_pixel(const int x, const int y) const;
        glm::vec2 pixel_to_vector(const SDL_Colour& colour) const;
};

class TileMap {
    std::vector<std::vector<entt::entity>> tilemap;

    public:
        entt::entity at(const int x, const int y);
        TileMap(entt::registry& registry);
        ~TileMap();
};

#endif