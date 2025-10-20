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
#include <tilespec.h>

class TileMap {
    private:
        entt::registry& m_registry;
        const int m_n_tiles;
        const TileSpec m_tile_spec;
        std::vector<entt::entity> m_tiles;
        std::optional<entt::entity> m_highlighted_tile;

    public:
        TileMap(entt::registry& registry, const int n_tiles, const glm::ivec2 tile_spec);
        ~TileMap();
        TileMap(const TileMap&) = delete;

        std::optional<entt::entity> operator[](const glm::ivec2 grid_position) const;
        void highlight_tile(const glm::ivec2 grid_position);
        const std::optional<entt::entity> highlighted_tile() const;
        const glm::ivec2 area() const;
        const glm::ivec2 origin_px() const;
        const TileSpec& tile_spec() const { return m_tile_spec; }
};

#endif