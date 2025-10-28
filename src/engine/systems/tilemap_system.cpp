#include <systems/tilemap_system.h>

void apply_highlight(entt::registry& registry, const Tile* tile, int factor=1) {
    for (entt::entity entity: tile->entities()) {
        TransformComponent& transform {registry.get<TransformComponent>(entity)};
        transform.position.y -= (30 * factor);
        transform.z_index += factor;
    }
}

void TileMapSystem::update(
    entt::registry& registry,
    const bool debug_mode
) {
    if (!debug_mode) {
        return;
    }

    TileMapComponent& tilemap {registry.ctx().get<TileMapComponent>()};
    const MouseComponent& mouse {registry.ctx().get<const MouseComponent>()};
    
    GridPosition grid_position {
        ScreenPosition(mouse.window_current_position).to_grid_position(registry)
    };

    if (mouse.moved) {
        if (tilemap.highlighted_tile) {
            apply_highlight(registry, tilemap.highlighted_tile, -1);
        }

        tilemap.highlighted_tile = nullptr;
        Tile* highlighted_tile {tilemap[grid_position]};

        if (highlighted_tile) {
            apply_highlight(registry, highlighted_tile);
            tilemap.highlighted_tile = highlighted_tile;
        }
    }
}