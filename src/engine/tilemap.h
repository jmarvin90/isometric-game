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
#include <memory>

#include <components/transform_component.h>
#include <tilespec.h>
#include <spritesheet.h>

struct Tile {
    entt::entity tile_entity;
    std::optional<entt::entity> building_entity;

    Tile(entt::registry& registry)
    : tile_entity {registry.create()}
    , building_entity {std::nullopt}
    {}

    Tile(const Tile&) = delete;
    Tile(Tile&&) = default;
    ~Tile() = default;
    bool has(const entt::entity) const;
    void highlight(entt::registry& registry, int factor);
};


class TileMap {
    private:
        entt::registry& m_registry;

    public:
        const int m_n_tiles;

    private:
        const TileSpec m_tile_spec;
        std::vector<Tile> m_tiles;
        Tile* m_highlighted_tile;

    public:
        TileMap(
            entt::registry& registry,
            const int n_tiles,
            const int tile_width,
            const int tile_depth,
            const SpriteSheet& spritesheet
        );
        ~TileMap();
        TileMap(const TileMap&) = delete;
        
        Tile* operator[](const glm::ivec2 grid_position);
        void highlight_tile(const glm::ivec2 grid_position);
        void reset_highlighted_tile();
        Tile* highlighted_tile();
        const glm::ivec2 area() const;
        const glm::ivec2 origin_px() const;
        const TileSpec& tile_spec() const { return m_tile_spec; }
};

#endif