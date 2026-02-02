#include <position.h>
#include <spatialmap_component.h>

#include <glm/glm.hpp>

entt::entity SpatialMapComponent::operator[](const int cell_number) const
{
    if (map.find(cell_number) == map.end()) {
        return entt::null;
    }

    return map.at(cell_number);
}

entt::entity SpatialMapComponent::operator[](const glm::ivec2 grid_position) const
{
    SpatialMapGridPosition grid_pos { grid_position };
    int cell_number { Position::to_spatial_map_cell(grid_pos, *this) };

    if (!Position::is_valid(grid_pos, *this)) {
        return entt::null;
    }

    return (*this)[cell_number];
}