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

    // TODO: why is it necessary to convert to a world position here
    int cell_number { grid_pos.to_spatial_map_cell(*this) };

    if (
        !grid_pos.is_valid(*this) || map.find(cell_number) == map.end()
    ) {
        return entt::null;
    }

    return (*this)[cell_number];
}