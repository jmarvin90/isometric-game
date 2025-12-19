#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <string>

namespace constants {
    inline constexpr glm::ivec2 CAMERA_SCROLL_SPEED { 10, 10 };
    inline constexpr int FPS { 60 };
    inline constexpr float MILLIS_PER_FRAME { 1'000.f / FPS };
    inline constexpr glm::ivec2 SCENE_BORDER_PX { 150, 150 };
    const std::string spritesheet { "assets/spritesheet_scaled.png" };
} // namespace constants

#endif