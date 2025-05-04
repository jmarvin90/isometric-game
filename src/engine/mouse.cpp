
#include <cmath>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <imgui.h>

#include "mouse.h"
#include "constants.h"

Mouse::Mouse(const MouseMap& mousemap) : mousemap{ mousemap }
{
    spdlog::info("Mouse constructor called.");
}

Mouse::~Mouse()
{
    spdlog::info("Mouse destructor called.");
}

void Mouse::update(const glm::ivec2& camera_position)
{
    // Update the current & previous window position
    window_previous_position = window_current_position;
    mouse_state = SDL_GetMouseState(
        &window_current_position.x,
        &window_current_position.y);

    // Update the world position
    world_position = window_current_position + camera_position;

    // Update the current grid position
    grid_position = mousemap.pixel_to_grid(world_position);
}

const glm::ivec2& Mouse::get_window_position() const
{
    return window_current_position;
}

const glm::ivec2& Mouse::get_world_position() const
{
    return world_position;
}

const glm::ivec2& Mouse::get_grid_position() const
{
    return grid_position;
}

const bool Mouse::has_moved_this_frame() const
{
    return window_previous_position != window_current_position;
}

const bool Mouse::is_on_world_grid() const
{
    return grid_position.x >= 0 &&
        grid_position.y >= 0 &&
        grid_position.x < constants::MAP_SIZE_N_TILES &&
        grid_position.y < constants::MAP_SIZE_N_TILES;
}

const uint32_t Mouse::get_mouse_state() const
{
    return mouse_state;
}

void Mouse::update_imgui_io(ImGuiIO& io) const
{
    io.MousePos = ImVec2(window_current_position.x, window_current_position.y);
    io.MouseDown[0] = mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT);
    io.MouseDown[1] = mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT);
}