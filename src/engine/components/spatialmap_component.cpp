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
    int index_pos { (grid_position.y * cells_per_row) + grid_position.x };

    if (
        !SpatialMapGridPosition(grid_position).is_valid(*this) || (cells_per_row * cells_per_row) <= index_pos
    ) {
        return entt::null;
    }

    return (*this)[index_pos];
}