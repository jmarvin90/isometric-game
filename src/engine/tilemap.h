#ifndef TILEMAP_H
#define TILEMAP_H


#include <vector>
#include <entt/entt.hpp>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <cmath>
#include <optional>
#include <spdlog/spdlog.h>
#include <functional>

#include <components/transform.h>

class TileMap;

class Tile {
    private:
        entt::entity m_entity;
        const TileMap& m_tilemap;
        const int tile_n;

        std::optional<glm::ivec2> m_grid_pos;
        std::optional<glm::ivec2> m_origin_px;
        std::optional<glm::ivec2> m_centre_px;

    public:
        Tile(entt::registry& registry, const TileMap& tilemap, const int tile_n);
        ~Tile();
        Tile(const Tile&) = delete;
        Tile& operator=(const TileMap&) = delete;
        Tile(Tile&&) noexcept = default;
        Tile& operator=(Tile&&) noexcept = default;

        const entt::entity entity() const { return m_entity; }
        const glm::ivec2& origin_px();
        const glm::ivec2& centre_px();
        const glm::ivec2& grid_pos();

        // For development - delete later
        void iso_sdl_points(std::array<SDL_Point, 5>& iso_sdl_points, const glm::ivec2& offset);
};

class TileMap {
    private:
        entt::registry& registry;
        const int m_n_tiles;
        const glm::ivec2 tile_spec;
        const glm::ivec2 tilemap_area;
        const glm::ivec2 origin_px;
        const glm::mat2 mat;
        const glm::mat2 mat_inv;
        std::vector<Tile> tiles;

    public:
        friend class Tile;

        TileMap(entt::registry& registry, const int n_tiles, const glm::ivec2 tile_spec);
        ~TileMap() = default;
        TileMap(const TileMap&) = delete;


        Tile* operator[](const glm::ivec2 world_position);

        const glm::ivec2& area() const { return tilemap_area; }

        // Consider deprecating
        const glm::ivec2 grid_to_world_px(const glm::ivec2 grid_pos) const;
        const glm::ivec2 world_px_to_grid(const glm::ivec2 world_pos) const;

        // For testing - to be deleted
        std::vector<Tile>& get_tiles() { return tiles; }

};

#endif