#include <spatialmap_component.h>

entt::entity SpatialMapComponent::operator[](const int cell_number) const
{
    if (map.find(cell_number) == map.end()) {
        return entt::null;
    }

    return map.at(cell_number);
}