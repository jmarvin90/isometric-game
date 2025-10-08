#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <vector>
#include <array>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

namespace constants {
    inline constexpr int FPS                            {60};
    inline constexpr float MILLIS_PER_FRAME             {1'000.f/FPS};
}

#endif