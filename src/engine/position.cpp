#include <SDL2/SDL.h>
#include <constants.h>
#include <position.h>

namespace Position {

glm::ivec2 screen_to_world(
    const glm::ivec2 screen_position,
    const glm::ivec2 camera_position
)
{
    return (screen_position + camera_position) - constants::SCENE_BORDER_PX;
}

glm::ivec2 world_to_screen(
    const glm::ivec2 world_position,
    const glm::ivec2 camera_position
)
{
    return (world_position - camera_position) + constants::SCENE_BORDER_PX;
}

} // namespace