#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

namespace constants {
    inline constexpr int FPS                            {60};
    inline constexpr float MILLIS_PER_FRAME             {1'000.f/FPS};
    const std::string spritesheet                       {"assets/spritesheet_scaled.png"};
}

#endif