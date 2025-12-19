#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <string>

namespace constants {
    inline constexpr int FPS { 60 };
    inline constexpr float MILLIS_PER_FRAME { 1'000.f / FPS };
    inline constexpr glm::ivec2 scene_border_px { 150, 150 };
    const std::string spritesheet { "assets/spritesheet_scaled.png" };
} // namespace constants

#endif