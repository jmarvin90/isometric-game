#include <systems/tilemap_system.h>

void apply_highlight(entt::registry& registry, const Tile* tile, int factor=1) {
    for (entt::entity entity: tile->entities()) {
        TransformComponent& transform {registry.get<TransformComponent>(entity)};
        transform.position.y -= (30 * factor);
        transform.z_index += factor;
    }
}

void TileMapSystem::update(entt::registry& registry, const bool debug_mode) {
    auto& tilemap = registry.ctx().get<TileMapComponent>();
    const auto& mouse = registry.ctx().get<const MouseComponent>();

    if (!debug_mode) {
        if (tilemap.highlighted_tile) {
            apply_highlight(registry, tilemap.highlighted_tile, -1);
            tilemap.highlighted_tile = nullptr;
        }
        return;
    }

    if (!mouse.moved) {
        return;
    }

    const GridPosition grid_position {
        ScreenPosition(mouse.window_current_position)
        .to_grid_position(registry)
    };

    Tile* new_tile = tilemap[grid_position];

    if (new_tile == tilemap.highlighted_tile) {
        return;
    }

    if (tilemap.highlighted_tile) {
        apply_highlight(registry, tilemap.highlighted_tile, -1);
    }

    if (new_tile) {
        apply_highlight(registry, new_tile);
    }

    tilemap.highlighted_tile = new_tile;
}