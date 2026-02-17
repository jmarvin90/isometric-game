#include <components/transform_component.h>
#include <glm/glm.hpp>
#include <spatialmap_component.h>

namespace {
// TODO: guaranteed to be the same as TileMapGridPosition
bool is_valid(const SpatialMapComponent& spatial_map, const glm::ivec2 grid_position)
{
    bool is_positive { glm::all(glm::greaterThanEqual(grid_position, glm::ivec2 { 0, 0 })) };
    bool is_in_bounds { glm::all(glm::lessThan(grid_position, glm::ivec2 { spatial_map.n_per_row, spatial_map.n_per_row })) };
    return is_positive && is_in_bounds;
}

int to_cell_number(const SpatialMapComponent& spatial_map, const glm::ivec2 grid_position)
{
    return (grid_position.y * spatial_map.n_per_row) + grid_position.x;
}
}

entt::entity SpatialMapComponent::operator[](const int cell_number) const
{
    if (map.find(cell_number) == map.end()) {
        return entt::null;
    }

    return map.at(cell_number);
}

entt::entity SpatialMapComponent::operator[](const glm::ivec2 grid_position) const
{
    int cell_number { to_cell_number(*this, grid_position) };

    if (!is_valid(*this, grid_position)) {
        return entt::null;
    }

    return (*this)[cell_number];
}

entt::entity SpatialMapComponent::operator[](const TransformComponent& transform) const
{
    // TODO: watch for potential redundancy grid position logic
    glm::ivec2 grid_position { glm::ivec2 { transform.position } / cell_size };
    return (*this)[grid_position];
}

void SpatialMapComponent::emplace_at(const glm::ivec2 grid_position, entt::entity entity)
{
    map[to_cell_number(*this, grid_position)] = entity;
}